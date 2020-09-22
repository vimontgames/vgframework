#include "graphics/driver/Precomp.h"
#include "Device.h"
#include "graphics/driver/CommandQueue/CommandQueue.h"
#include "graphics/driver/CommandPool/CommandPool.h"
#include "graphics/driver/CommandList/CommandList.h"
#include "graphics/driver/Resource/Texture.h"
#include "graphics/driver/Resource/Buffer.h"
#include "graphics/driver/Memory/MemoryAllocator.h"
#include "graphics/driver/RootSignature/RootSignature.h"
#include "graphics/driver/Shader/ShaderManager.h"
#include "graphics/driver/FrameGraph/RenderPass.h"

using namespace vg::core;
using namespace vg::graphics;

#include VG_GFXAPI_IMPL(Device)

namespace vg::graphics::driver
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
            m_shaderManager = new driver::ShaderManager("data/shaders");
		}

		//--------------------------------------------------------------------------------------
		void Device::deinit()
		{
            VG_SAFE_DELETE(m_shaderManager);
		}

		//--------------------------------------------------------------------------------------
		void Device::beginFrame()
		{

		}

		//--------------------------------------------------------------------------------------
		void Device::endFrame()
		{
            m_frameCounter++;
		}

		//--------------------------------------------------------------------------------------
		driver::Device * Device::getDevice()
		{
			return static_cast<driver::Device*>(this);
		}

		//--------------------------------------------------------------------------------------
		const DeviceParams & Device::getDeviceParams() const
		{
			return m_deviceParams;
		}

		//--------------------------------------------------------------------------------------
		driver::CommandQueue * Device::createCommandQueue(CommandQueueType _type)
		{
            auto *& queue = m_commandQueue[asInteger(_type)];
            VG_ASSERT(nullptr == queue);
            queue = new driver::CommandQueue(_type);
			return queue;
		}

		//--------------------------------------------------------------------------------------
		driver::CommandQueue * Device::getCommandQueue(CommandQueueType _type)
		{
			VG_ASSERT(isEnumValue(_type));
			return m_commandQueue[asInteger(_type)];
		}

		//--------------------------------------------------------------------------------------
		core::vector<driver::CommandList*> & Device::getCommandLists(CommandListType _type)
		{
			VG_ASSERT(isEnumValue(_type));
			return getCurrentFrameContext().commandLists[asInteger(_type)];
		}

		//--------------------------------------------------------------------------------------
		void Device::createFrameContext(core::uint _frameContextIndex, void * _surface)
		{
			auto & context = getFrameContext(_frameContextIndex);

			// Create backbuffer texture
			{
				TextureDesc backbufferTexDesc
				(
                    Usage::Default,
                    BindFlags::None,
                    CPUAccessFlags::None,
					TextureType::Texture2D,
					m_backbufferFormat,
					TextureFlags::RenderTarget | TextureFlags::Backbuffer | TextureFlags::sRGB,
					getDeviceParams().resolution.x,
					getDeviceParams().resolution.y
                );

				char backbufferName[sizeof("Backbuffer")+2];
				sprintf_s(backbufferName, "Backbuffer#%u", _frameContextIndex);

				m_frameContext[_frameContextIndex].backbuffer = getDevice()->createTexture(backbufferTexDesc, backbufferName, _surface);
			}

			// Create command lists
			{
                auto & cmdPools = context.commandPools;

                // Create graphics command list for uploads
                {
                    const auto cmdPoolIndex = (uint)cmdPools.size();
                    cmdPools.push_back(new driver::CommandPool(_frameContextIndex, cmdPoolIndex));
                    auto & cmdPool = cmdPools.back();

                    auto & cmdLists = context.commandLists[asInteger(CommandListType::Graphics)];
                    const auto cmdListIndex = (uint)cmdLists.size();
                    cmdLists.push_back(new driver::CommandList(CommandListType::Graphics, cmdPool, _frameContextIndex, cmdListIndex));
                }

				// Create default Graphics command list
				{
                    const auto cmdPoolIndex = (uint)cmdPools.size();
                    cmdPools.push_back(new driver::CommandPool(_frameContextIndex, cmdPoolIndex));
                    auto & cmdPool = cmdPools.back();

					auto & cmdLists = context.commandLists[asInteger(CommandListType::Graphics)];
					const auto cmdListIndex = (uint)cmdLists.size();
					cmdLists.push_back(new driver::CommandList(CommandListType::Graphics, cmdPool, _frameContextIndex, cmdListIndex));
				}
			}

            // Buffer for uploads
            {
                auto & buffer = context.m_uploadBuffer;

                BufferDesc bufDesc = BufferDesc(Usage::Upload, BindFlags::None, CPUAccessFlags::Write, BufferFlags::None, 4 * 1024);
                char uploadBufferName[sizeof("Upload")+2];
                sprintf_s(uploadBufferName, "Upload#%u", _frameContextIndex);
                buffer = new driver::Buffer(bufDesc, uploadBufferName);

                // keep always mapped
                Map result = buffer->getResource().map();
                context.m_uploadBegin = (core::u8*)result.data;
                context.m_uploadCur = context.m_uploadBegin;
            }
		}

		//--------------------------------------------------------------------------------------
		void Device::destroyFrameContext(core::uint _frameContextIndex)
		{
            auto & frameContext = m_frameContext[_frameContextIndex];
			auto & pools = frameContext.commandPools;
			for (auto & pool : pools)
				VG_SAFE_RELEASE(pool);
			pools.clear();

			for (uint type = 0; type < enumCount<CommandListType>(); ++type)
			{
				auto & cmdLists = frameContext.commandLists[type];
				for (auto & cmdList : cmdLists)
					VG_SAFE_RELEASE(cmdList);
				cmdLists.clear();
			}

			VG_SAFE_RELEASE(frameContext.backbuffer);

            for (Object * obj : frameContext.m_objectsToRelease)
                VG_SAFE_RELEASE(obj);
            frameContext.m_objectsToRelease.clear();

            frameContext.m_uploadBuffer->getResource().unmap();
            VG_SAFE_RELEASE(frameContext.m_uploadBuffer);
		}

		//--------------------------------------------------------------------------------------
		FrameContext & Device::getFrameContext(uint _frameContextIndex)
		{
			VG_ASSERT(_frameContextIndex < countof(m_frameContext));
			return m_frameContext[_frameContextIndex];
		}

        //--------------------------------------------------------------------------------------
        u64 Device::getFrameCounter() const
        {
            return m_frameCounter;
        }
        //--------------------------------------------------------------------------------------
        uint Device::getFrameContextIndex() const
        {
            return m_frameCounter % max_frame_latency;
        }

		//--------------------------------------------------------------------------------------
		FrameContext & Device::getCurrentFrameContext()
		{
			return getFrameContext(getFrameContextIndex());
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
		driver::Texture * Device::getBackbuffer()
		{
			return getCurrentFrameContext().backbuffer;
		}

        //--------------------------------------------------------------------------------------
        void Device::upload(driver::Texture * _dst, core::u32 _from)
        {
            auto & context = getCurrentFrameContext();
            context.m_texturesToUpload.push_back({ _dst, _from });
        }
	}

	//--------------------------------------------------------------------------------------
	void Device::init(const DeviceParams & _params)
	{
		Super::init(_params);
	}

	//--------------------------------------------------------------------------------------
	void Device::deinit()
	{
		// [...]

		Super::deinit();
	}

	//--------------------------------------------------------------------------------------
	void Device::beginFrame()
	{
        #if VG_DBG_CPUGPUSYNC
        VG_DEBUGPRINT("beginFrame #%u\n{\n", getFrameCounter());
        #endif

		Super::beginFrame();

        // Copy staging data to GPU textures used for rendering
        flushTextureUploads();

        // It is safe now to release frame (n-max_frame_latency+1) resources as we are now sure they are not in use by the GPU
        flushReleaseAsync();

        #if VG_DBG_CPUGPUSYNC
        VG_DEBUGPRINT("[...]\n");
        #endif
	}
	
	//--------------------------------------------------------------------------------------
	void Device::endFrame()
	{
		Super::endFrame();

        #if VG_DBG_CPUGPUSYNC
        VG_DEBUGPRINT("}\nendFrame #%u\n", getFrameCounter());
        #endif
	}

    //--------------------------------------------------------------------------------------
    void Device::flushTextureUploads()
    {
        auto & context = getCurrentFrameContext();
        auto * cmdList = context.commandLists[asInteger(CommandQueueType::Graphics)][0];
        auto & textures = context.m_texturesToUpload;

        for (uint i = 0; i < textures.size(); ++i)
        {
            auto & pair = textures[i];
            cmdList->copyTexture(pair.first, pair.second);
        }

        textures.clear();
        context.m_uploadCur = context.m_uploadBegin;
    }

	//--------------------------------------------------------------------------------------
	Texture * Device::createTexture(const TextureDesc & _texDesc, const core::string & _name, void * _initData)
	{
		return new Texture(_texDesc, _name, _initData);
	}

    //--------------------------------------------------------------------------------------
    Buffer * Device::createBuffer(const BufferDesc & _bufDesc, const core::string & _name, void * _initData)
    {
        return new Buffer(_bufDesc, _name, _initData);
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
}