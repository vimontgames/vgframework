#include "gfx/Precomp.h"
#include "Device.h"
#include "gfx/CommandQueue/CommandQueue.h"
#include "gfx/CommandPool/CommandPool.h"
#include "gfx/CommandList/CommandList.h"
#include "gfx/Resource/Texture.h"
#include "gfx/Resource/Buffer.h"
#include "gfx/Memory/MemoryAllocator.h"
#include "gfx/RootSignature/RootSignature.h"
#include "gfx/Shader/ShaderManager.h"
#include "gfx/FrameGraph/RenderPass.h"
#include "gfx/BindlessTable/BindlessTable.h"
#include "gfx/PipelineState/SamplerState.h"
#include "gfx/Profiler/Profiler.h"
#include "gfx/Importer/TextureImporter.h"
#include "gfx/Resource/UploadBuffer.h"

#include "gfx/Device/DeviceCaps.hpp"

using namespace vg::core;
using namespace vg::gfx;

#include VG_GFXAPI_IMPL(Device)

namespace vg::gfx
{
	namespace base
	{
		//--------------------------------------------------------------------------------------
		Device::Device() :
            m_backbufferFormat(PixelFormat::Unknow)
		{

		}

		//--------------------------------------------------------------------------------------
		Device::~Device()
		{

		}

		//--------------------------------------------------------------------------------------
		void Device::init(const DeviceParams & _params)
		{
			m_deviceParams = _params;
            m_shaderManager = new gfx::ShaderManager("data/shaders");
		}

		//--------------------------------------------------------------------------------------
		void Device::deinit()
		{
            VG_SAFE_DELETE(m_shaderManager);
		}

		//--------------------------------------------------------------------------------------
		void Device::beginFrame()
		{
            m_bindlessTable->beginFrame();
		}

		//--------------------------------------------------------------------------------------
		void Device::endFrame()
		{
            m_frameCounter++;
		}

		//--------------------------------------------------------------------------------------
		gfx::Device * Device::getDevice()
		{
			return static_cast<gfx::Device*>(this);
		}

		//--------------------------------------------------------------------------------------
		const DeviceParams & Device::getDeviceParams() const
		{
			return m_deviceParams;
		}

		//--------------------------------------------------------------------------------------
		gfx::CommandQueue * Device::createCommandQueue(CommandQueueType _type)
		{
            auto *& queue = m_commandQueue[asInteger(_type)];
            VG_ASSERT(nullptr == queue);
            queue = new gfx::CommandQueue(_type);
			return queue;
		}

		//--------------------------------------------------------------------------------------
		gfx::CommandQueue * Device::getCommandQueue(CommandQueueType _type)
		{
			VG_ASSERT(isEnumValue(_type));
			return m_commandQueue[asInteger(_type)];
		}

		//--------------------------------------------------------------------------------------
		core::vector<gfx::CommandList*> & Device::getCommandLists(CommandListType _type)
		{
			VG_ASSERT(isEnumValue(_type));
			return getCurrentFrameContext().commandLists[asInteger(_type)];
		}

        //--------------------------------------------------------------------------------------
        void Device::createUploadBuffer()
        {
            m_uploadBuffer = new UploadBuffer("Upload", 512 * 1024 * 1024); // must implement upload limit per frame but for now just temporarily increase the upload buffer size 
        }

        //--------------------------------------------------------------------------------------
        void Device::destroyUploadBuffer()
        {
            VG_SAFE_DELETE(m_uploadBuffer);
        }

        //--------------------------------------------------------------------------------------
        UploadBuffer * Device::getUploadBuffer()
        {
            return m_uploadBuffer;
        }

		//--------------------------------------------------------------------------------------
		void Device::createFrameContext(core::uint _frameContextIndex)
		{
			auto & context = m_frameContext[_frameContextIndex];

            context.mFrameFenceId = 0;

			// Create command lists
			{
                auto & cmdPools = context.commandPools;

                // Create graphics command list for uploads
                //{
                //    const auto cmdPoolIndex = (uint)cmdPools.size();
                //    cmdPools.push_back(new gfx::CommandPool(_frameContextIndex, cmdPoolIndex));
                //    auto & cmdPool = cmdPools.back();
                //
                //    auto & cmdLists = context.commandLists[asInteger(CommandListType::Graphics)];
                //    const auto cmdListIndex = (uint)cmdLists.size();
                //    cmdLists.push_back(new gfx::CommandList(CommandListType::Graphics, cmdPool, _frameContextIndex, cmdListIndex));
                //}

				// Create default Graphics command list
				{
                    const auto cmdPoolIndex = (uint)cmdPools.size();
                    cmdPools.push_back(new gfx::CommandPool(_frameContextIndex, cmdPoolIndex));
                    auto & cmdPool = cmdPools.back();

					auto & cmdLists = context.commandLists[asInteger(CommandListType::Graphics)];
					const auto cmdListIndex = (uint)cmdLists.size();
					cmdLists.push_back(new gfx::CommandList(CommandListType::Graphics, cmdPool, _frameContextIndex, cmdListIndex));
				}
			}
		}

		//--------------------------------------------------------------------------------------
		void Device::destroyFrameContext(core::uint _frameContextIndex)
		{
            auto & context = m_frameContext[_frameContextIndex];
			auto & pools = context.commandPools;
			for (auto & pool : pools)
				VG_SAFE_RELEASE(pool);
			pools.clear();

			for (uint type = 0; type < enumCount<CommandListType>(); ++type)
			{
				auto & cmdLists = context.commandLists[type];
				for (auto & cmdList : cmdLists)
					VG_SAFE_RELEASE(cmdList);
				cmdLists.clear();
			}			

            for (Object * obj : context.m_objectsToRelease)
                VG_SAFE_RELEASE(obj);
            context.m_objectsToRelease.clear();
		}

        //--------------------------------------------------------------------------------------
        uint Device::getFrameContextIndex() const
        {
            return m_currentFrameIndex;
        }

        //--------------------------------------------------------------------------------------
        FrameContext & Device::getCurrentFrameContext()
        {
            return m_frameContext[m_currentFrameIndex];
        }

        //--------------------------------------------------------------------------------------
        void Device::createBackbuffer(core::uint _backbufferIndex, void * _backbuffer)
        {
             TextureDesc backbufferTexDesc
            (
                Usage::Default,
                BindFlags::None,
                CPUAccessFlags::None,
                TextureType::Texture2D,
                m_backbufferFormat,
                TextureFlags::RenderTarget | TextureFlags::Backbuffer,
                getDeviceParams().resolution.x,
                getDeviceParams().resolution.y
            );

            m_bufferContext[_backbufferIndex].backbuffer = getDevice()->createTexture(backbufferTexDesc, "Backbuffer#" + to_string(_backbufferIndex), _backbuffer);
        }

        //--------------------------------------------------------------------------------------
        void Device::destroyBackbuffer(core::uint _backbufferIndex)
        {
            VG_SAFE_RELEASE(m_bufferContext[_backbufferIndex].backbuffer);
        }

        //--------------------------------------------------------------------------------------
        core::uint Device::getBackbufferIndex() const
        {
            return m_currentBackbufferIndex;
        }

        //--------------------------------------------------------------------------------------
        BufferContext & Device::getCurrentBackbuffer()
        {
            return m_bufferContext[m_currentBackbufferIndex];
        }

        //--------------------------------------------------------------------------------------
        u64 Device::getFrameCounter() const
        {
            return m_frameCounter;
        }

        //--------------------------------------------------------------------------------------
        void Device::releaseAsync(core::Object * _object)
        {
            getCurrentFrameContext().m_objectsToRelease.push_back(_object);
        }

        //--------------------------------------------------------------------------------------
        void Device::flushReleaseAsync()
        {
            auto & frameContext = m_frameContext[getFrameContextIndex()];

            #if VG_DBG_CPUGPUSYNC
            if (frameContext.m_objectsToRelease.size())
                VG_DEBUGPRINT("Release %u object(s) async [%u]\n", frameContext.m_objectsToRelease.size(), getFrameContextIndex());
            #endif

            for (Object * obj : frameContext.m_objectsToRelease)
                VG_SAFE_RELEASE(obj);
            frameContext.m_objectsToRelease.clear();
        }

		//--------------------------------------------------------------------------------------
		void Device::destroyCommandQueues()
		{
			for (uint queueType = 0; queueType < enumCount<CommandQueueType>(); ++queueType)
                VG_SAFE_RELEASE(m_commandQueue[queueType]);
		}

		//--------------------------------------------------------------------------------------
		gfx::Texture * Device::getBackbuffer()
		{
            auto * backbuffer = m_bufferContext[m_currentBackbufferIndex].backbuffer;
            VG_ASSERT(backbuffer);
            return backbuffer;
		}

        //--------------------------------------------------------------------------------------
        gfx::BindlessTable * Device::getBindlessTable() const
        {
            VG_ASSERT(m_bindlessTable);
            return m_bindlessTable;
        }
	}

	//--------------------------------------------------------------------------------------
	void Device::init(const DeviceParams & _params)
	{
		super::init(_params);
		m_caps.init();
        VG_PROFILE_INIT();
        m_textureImporter = new TextureImporter();
        m_bindlessTable->init();
	}

	//--------------------------------------------------------------------------------------
	void Device::deinit()
	{
        waitGPUIdle();
        VG_SAFE_DELETE(m_textureImporter);
        VG_PROFILE_DEINIT();
		super::deinit();
	}

    //--------------------------------------------------------------------------------------
    void Device::resize(core::uint _width, core::uint _height)
    {
        VG_DEBUGPRINT("[Device] resize(%u, %u)\n", _width, _height);
        m_deviceParams.resolution = uint2(_width, _height);

        if (isMinimized())
            return;

        super::resize(_width, _height);

        waitGPUIdle();
    }

    //--------------------------------------------------------------------------------------
    bool Device::isMinimized()
    {
        return any(0 == m_deviceParams.resolution); 
    }

    //--------------------------------------------------------------------------------------
    void Device::resetShaders(ShaderKey::File _file)
    {
        waitGPUIdle();

        for (FrameContext & frame : m_frameContext)
        {
            for (auto cmdListType = 0; cmdListType < enumCount<CommandListType>(); ++cmdListType)
            {
                auto & cmdLists = frame.commandLists[cmdListType];
                for (auto * cmdList : cmdLists)
                    cmdList->resetShaders(_file);
            }
        }
    }

	//--------------------------------------------------------------------------------------
	void Device::beginFrame()
	{
        VG_PROFILE_CPU("BeginFrame");

        #if VG_DBG_CPUGPUSYNC
        VG_DEBUGPRINT("beginFrame #%u\n{\n", getFrameCounter());
        #endif

		super::beginFrame();

        // Copy staging data to GPU textures used for rendering
        flushUploadBuffer();

        // It is safe now to release frame (n-max_frame_latency+1) resources as we are now sure they are not in use by the GPU
        flushReleaseAsync();

        #if VG_DBG_CPUGPUSYNC
        VG_DEBUGPRINT("[...]\n");
        #endif
	}
	
	//--------------------------------------------------------------------------------------
	void Device::endFrame()
	{
        VG_PROFILE_CPU("EndFrame");

		super::endFrame();

        #if VG_DBG_CPUGPUSYNC
        VG_DEBUGPRINT("}\nendFrame #%u\n", getFrameCounter());
        #endif
	}

    //--------------------------------------------------------------------------------------
    void Device::flushUploadBuffer()
    {
        VG_PROFILE_CPU("flushTextureUploads");

        auto & context = getCurrentFrameContext();
        auto * cmdList = context.commandLists[asInteger(CommandQueueType::Graphics)][0];

        m_uploadBuffer->flush(cmdList);
    }

	//--------------------------------------------------------------------------------------
	Texture * Device::createTexture(const TextureDesc & _texDesc, const core::string & _name, const void * _initData, ReservedSlot _reservedSlot)
	{
        VG_ASSERT(_texDesc.width > 0 && _texDesc.height > 0);
		return new Texture(_texDesc, _name, _initData, _reservedSlot);
	}

    //--------------------------------------------------------------------------------------
    Texture * Device::createTexture(const core::string & _path, ReservedSlot _reservedSlot)
    {
        TextureDesc texDesc;
        core::vector<u8> texData;

        if (m_textureImporter->importTextureData(_path, texDesc, texData))
        {
            Texture * tex = createTexture(texDesc, _path, texData.data(), _reservedSlot);
            return tex;
        }
        else
        {
            VG_DEBUGPRINT("[Device] Failed to create texture from \"%s\"\n", _path.c_str());
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    Buffer * Device::createBuffer(const BufferDesc & _bufDesc, const core::string & _name, const void * _initData, ReservedSlot _reservedSlot)
    {
        return new Buffer(_bufDesc, _name, _initData, _reservedSlot);
    }

    //--------------------------------------------------------------------------------------
    RootSignatureHandle Device::addRootSignature(const RootSignatureDesc & _desc)
    {
        return m_rootSignaturesTable.add(_desc);
    }

    //--------------------------------------------------------------------------------------
    RootSignature * Device::getRootSignature(const RootSignatureHandle & _handle)
    {
        return m_rootSignaturesTable.get(_handle);
    }

    //--------------------------------------------------------------------------------------
    uint Device::removeRootSignature(RootSignatureHandle & _handle)
    {
        return m_rootSignaturesTable.remove(_handle);
    }

    //--------------------------------------------------------------------------------------
    void Device::setVSync(VSync mode)
    {
        if (mode != m_VSync)
        {
            m_VSync = mode;
            super::setVSync(mode);
        }
    }

    //--------------------------------------------------------------------------------------
    VSync Device::getVSync() const
    {
        return m_VSync;
    }
}