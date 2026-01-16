#include "gfx/Precomp.h"
#include "Device.h"
#include "core/Timer/Timer.h"
#include "core/Memory/MemoryInfo.h"
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
#include "gfx/PipelineState/Graphic/SamplerState.h"
#include "gfx/Profiler/Profiler.h"
#include "gfx/Importer/TextureImporter.h"
#include "gfx/UploadBuffer/UploadBuffer.h"
#include "gfx/Importer/TextureImporterSettings.h"

#if !VG_ENABLE_INLINE
#include "Device.inl"
#endif

#include "gfx/Device/DeviceCaps.hpp"
#include "DXGIHelper.hpp"

#ifdef VG_DX12
#include "dx12/Device_dx12.hpp"
#elif defined(VG_VULKAN)
#include "vulkan/Device_vulkan.hpp"
#else
#error Undefined GFXAPI
#endif

using namespace vg::core;

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
            m_shaderManager = new gfx::ShaderManager("data/Shaders", _params.api);
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
        void Device::setGpuFrameTime(double _gpuFrameTime)
        {
            m_gpuFrameTime = _gpuFrameTime;
        }

        //--------------------------------------------------------------------------------------
        void Device::beginCapture()
        {
            m_captureInProgress = true;
        }

        //--------------------------------------------------------------------------------------
        void Device::endCapture()
        {
            m_captureInProgress = false;
        }

        //--------------------------------------------------------------------------------------
        void Device::beginWaitGPU()
        {
            m_beginWaitGPUTicks = core::Timer::getTick();
        }

        //--------------------------------------------------------------------------------------
        void Device::endWaitGPU()
        {
            m_gpuWaitTime = core::Timer::getEnlapsedTime(m_beginWaitGPUTicks, core::Timer::getTick());
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
			return getCurrentFrameContext().m_commandLists[asInteger(_type)];
		}

        //--------------------------------------------------------------------------------------
        void Device::setExecuteCommandListCount(CommandListType _type, core::uint _count)
        {
            auto & frameContext = getCurrentFrameContext();
            const uint cmdListCount = (uint)frameContext.m_commandLists[asInteger(_type)].size();
            VG_ASSERT(_count <= cmdListCount);
            frameContext.m_executeCommandListCount[asInteger(_type)] = min(_count, cmdListCount);
        }

        //--------------------------------------------------------------------------------------
        void Device::createStreamingUploadBuffer()
        {
            VG_ASSERT(m_uploadBuffers.size() == 0);
            size_t streamingUploadBufferSize = ((gfx::Device *)this)->getStreamingUploadBufferSize();
            m_uploadBuffers.push_back(new UploadBuffer("Streaming Upload Buffer", streamingUploadBufferSize, 0));
        }       

        //--------------------------------------------------------------------------------------
        void Device::createMainThreadUploadBuffer()
        {
            VG_ASSERT(m_uploadBuffers.size() == 1);
            size_t mainThreadUploadBufferSize = ((gfx::Device *)this)->getStreamingUploadBufferSize(); // dedicated setting
            m_uploadBuffers.push_back(new UploadBuffer("MainThread Upload Buffer", mainThreadUploadBufferSize, 1));
        }

        //--------------------------------------------------------------------------------------
        // One big upload buffer used for cmdlist #0 
        // + 1 'main' upload buffer used to create resources on the main thread
        // + 1 smaller upload buffer per other command list used for render
        //--------------------------------------------------------------------------------------
        void Device::updateCommandListsUploadBuffers()
        {
            if (m_renderJobsDirty)
            {
                // destroy all existing buffers but the 2 first ones used by uploads and main thread
                for (uint i = 2; i < (uint)m_uploadBuffers.size(); ++i)
                    VG_SAFE_RELEASE_ASYNC(m_uploadBuffers[i]);
                m_uploadBuffers.resize(2);                

                // recreate N buffers (2 to N+2)
                VG_ASSERT(-1 != m_maxRenderJobCount);
                const auto uploadBufferTargetCount = max((uint)2, m_maxRenderJobCount + 2);
                m_uploadBuffers.reserve(uploadBufferTargetCount);

                if (uploadBufferTargetCount > 2)
                {
                    const size_t uploadBufferSize = max(m_renderJobsWorkerMinBufferSize, m_renderJobsTotalBufferSize / (uploadBufferTargetCount-2));

                    for (uint i = (uint)2; i < uploadBufferTargetCount; ++i)
                        m_uploadBuffers.push_back(new UploadBuffer(fmt::sprintf("Worker #%u Upload Buffer", i-2), uploadBufferSize, i));
                }

                m_renderJobsDirty = false;
            }
        }

        //--------------------------------------------------------------------------------------
        void Device::destroyUploadBuffers()
        {
            for (uint i = 0; i < m_uploadBuffers.size(); ++i)
                VG_SAFE_DELETE(m_uploadBuffers[i]);
            m_uploadBuffers.clear();
        }

        //--------------------------------------------------------------------------------------
        UploadBuffer * Device::getStreamingUploadBuffer() const
        {
            return m_uploadBuffers[0];
        }

        //--------------------------------------------------------------------------------------
        UploadBuffer * Device::getMainThreadUploadBuffer() const
        {
            return m_uploadBuffers[1];
        }

        //--------------------------------------------------------------------------------------
        UploadBuffer * Device::getCommandListUploadBuffer(core::uint _cmdListIndex) const
        {
            const auto * scheduler = Kernel::getScheduler();
            const auto currentThread = scheduler->GetCurrentThreadType();

            switch (currentThread)
            {
                case ThreadType::Loading:
                    VG_ASSERT(_cmdListIndex == 0);
                    break;

                case ThreadType::Main:
                    //VG_ASSERT(_cmdListIndex == 1);
                    break;

                default:
                    VG_ASSERT(_cmdListIndex >= 2);
                    VG_ASSERT(_cmdListIndex < m_uploadBuffers.size());
                    break;
            }
            
            return m_uploadBuffers[_cmdListIndex];
        }

        //--------------------------------------------------------------------------------------
        UploadBuffer * Device::getCurrentUploadBuffer() const
        {
            const auto * scheduler = Kernel::getScheduler();
            const auto currentThread = scheduler->GetCurrentThreadType();

            UploadBuffer * uploadBuffer = nullptr;
            
            switch (currentThread)
            {
                case ThreadType::Loading:
                    uploadBuffer = getStreamingUploadBuffer();
                    break;

                case ThreadType::Main:
                    uploadBuffer = getMainThreadUploadBuffer();
                    break;

                default:
                    VG_ASSERT(currentThread == ThreadType::Main || currentThread == ThreadType::Loading, "Expected Main or Loading thread but current thread is \"%s\"", scheduler->GetCurrentThreadName().c_str());
            }

            return uploadBuffer;
        }

		//--------------------------------------------------------------------------------------
		void Device::createFrameContext(core::uint _frameContextIndex)
		{
			auto & context = m_frameContext[_frameContextIndex];

            context.m_frameFenceId = 0;

			// Create command lists
			{
                auto & cmdPools = context.m_commandPools;

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

					auto & cmdLists = context.m_commandLists[asInteger(CommandListType::Graphics)];
					const auto cmdListIndex = (uint)cmdLists.size();
                    VG_ASSERT(0 == cmdPoolIndex);
                    auto * cmdList = new gfx::CommandList(CommandListType::Graphics, cmdPool, _frameContextIndex, cmdListIndex);
					cmdLists.push_back(cmdList);
				}
			}
		}

        //--------------------------------------------------------------------------------------
        void Device::updateFrameContext()
        {
            auto frameContextIndex = getFrameContextIndex();
            auto & frameContext = m_frameContext[frameContextIndex];

            auto & cmdPools = frameContext.m_commandPools;
            auto & cmdLists = frameContext.m_commandLists[asInteger(CommandListType::Graphics)];
            VG_ASSERT(cmdPools.size() == cmdLists.size());

            // Alloc more command lists for render jobs if needed (Make sure we have as much CommandLists as worker threads)
            VG_ASSERT(-1 != m_maxRenderJobCount);
            const auto cmdListTargetCount = max((uint)2, m_maxRenderJobCount + 2);

            for (uint i = (uint)cmdPools.size(); i < cmdListTargetCount; ++i)
            {
                const auto cmdPoolIndex = (uint)cmdPools.size();
                cmdPools.push_back(new gfx::CommandPool(frameContextIndex, cmdPoolIndex));
                auto & cmdPool = cmdPools.back();

                const auto cmdListIndex = (uint)cmdLists.size();
                auto * cmdList = new gfx::CommandList(CommandListType::Graphics, cmdPool, frameContextIndex, cmdListIndex);
                cmdLists.push_back(cmdList);
            }
           
            // Destroy extra command lists
            for (uint i = cmdListTargetCount; i < (uint)cmdPools.size(); ++i)
            {
                VG_SAFE_RELEASE_ASYNC(cmdPools[i]);
                VG_SAFE_RELEASE_ASYNC(cmdLists[i]);
            }

            if (cmdPools.size() != cmdListTargetCount)
            {
                cmdPools.resize(cmdListTargetCount);
                cmdLists.resize(cmdListTargetCount);
            }            
        }

		//--------------------------------------------------------------------------------------
		void Device::destroyFrameContext(core::uint _frameContextIndex)
		{
            auto & context = m_frameContext[_frameContextIndex];
			auto & pools = context.m_commandPools;
			for (auto & pool : pools)
				VG_SAFE_RELEASE(pool);
			pools.clear();

			for (uint type = 0; type < enumCount<CommandListType>(); ++type)
			{
				auto & cmdLists = context.m_commandLists[type];
				for (auto & cmdList : cmdLists)
					VG_SAFE_RELEASE(cmdList);
				cmdLists.clear();
			}			

            Kernel::getFactory()->FlushReleaseAsync();
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

        //--------------------------------------------------------------------------------------
        PixelFormat Device::getHDRBackbufferFormat(HDR _mode) const
        {
            switch (_mode)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(_mode);

                case HDR::None:
                    return PixelFormat::R8G8B8A8_unorm;

                case HDR::HDR10:
                    return PixelFormat::R10G10B10A2_unorm;

                case HDR::HDR16:
                    return PixelFormat::R16G16B16A16_float;
            }
        }

        //--------------------------------------------------------------------------------------
        ColorSpace Device::getHDRColorSpace(HDR _mode) const
        {
            switch (_mode)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(_mode);

                case HDR::None:
                    return ColorSpace::Rec709;

                case HDR::HDR10:
                    return ColorSpace::ST2084;

                case HDR::HDR16:
                    return ColorSpace::Rec2020;
            }
        }

        //--------------------------------------------------------------------------------------
        void Device::setFullscreenMode(FullscreenMode _fullscreenMode)
        {
            m_fullscreenMode = _fullscreenMode;
        }

        //--------------------------------------------------------------------------------------
        FullscreenMode Device::getFullscreenMode() const
        {
            return m_fullscreenMode;
        }
	}

    #if VG_VULKAN
    #define STRINGIFY(x) #x
    #define EXPAND_AND_STRINGIFY(x) STRINGIFY(x)

    #if VK_HEADER_VERSION < 290
    #pragma message("Error: Current Vulkan SDK version " EXPAND_AND_STRINGIFY(VK_HEADER_VERSION) " is too old. Vulkan SDK version >=290 is required.")
    #error Vulkan SDK version >= 290 is required
    #endif
    #endif

	//--------------------------------------------------------------------------------------
	void Device::init(const DeviceParams & _params)
	{
        VG_PROFILE_CPU("Device");   

        m_streamingUploadBufferSize = _params.streamingUploadBufferSizeInMB * 1024 * 1024;

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
        m_deviceParams.resolution = uint2(_width, _height);

        if (isMinimized())
            return;

        VG_INFO("[Renderer] Resize to %ux%u", _width, _height);

        waitGPUIdle();
        super::resize(_width, _height);
    }

    //--------------------------------------------------------------------------------------
    bool Device::isMinimized()
    {
        return any(0 == m_deviceParams.resolution); 
    }

    //--------------------------------------------------------------------------------------
    void Device::resetShaders(ShaderKey::File _file)
    {
        for (FrameContext & frame : m_frameContext)
        {
            for (auto cmdListType = 0; cmdListType < enumCount<CommandListType>(); ++cmdListType)
            {
                auto & cmdLists = frame.m_commandLists[cmdListType];
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

        // Copy staging data to GPU textures/buffers used for rendering
        flushUploadBuffer();

        // It is safe now to release frame (n-max_frame_latency+1) resources as we are now sure they are not in use by the GPU
        Kernel::getFactory()->FlushReleaseAsync();

        #if VG_DBG_CPUGPUSYNC
        VG_DEBUGPRINT("[...]\n");
        #endif
	}
	
	//--------------------------------------------------------------------------------------
	void Device::endFrame()
	{
        VG_PROFILE_CPU("EndFrame");

		super::endFrame();

        for (uint i = 0; i < m_uploadBuffers.size(); ++i)
            m_uploadBuffers[i]->sync();

        getShaderManager()->applyUpdate();

        #if VG_DBG_CPUGPUSYNC
        VG_DEBUGPRINT("}\nendFrame #%u\n", getFrameCounter());
        #endif
	}

    //--------------------------------------------------------------------------------------
    void Device::flushUploadBuffer()
    {
        VG_PROFILE_CPU("flushTextureUploads");

        auto & context = getCurrentFrameContext();

        auto & cmdLists = context.m_commandLists[asInteger(CommandQueueType::Graphics)];

        for (uint i = 0; i < cmdLists.size(); ++i)
            m_uploadBuffers[i]->flush(cmdLists[i], true);
    }

	//--------------------------------------------------------------------------------------
	Texture * Device::createTexture(const TextureDesc & _texDesc, const core::string & _name, const void * _initData, ReservedSlot _reservedSlot, CommandList * _cmdList)
	{
        VG_ASSERT(_texDesc.width > 0 && _texDesc.height > 0);
        VG_PROFILE_CPU("createTexture");
		return new Texture(_texDesc, _name, _initData, _reservedSlot, _cmdList);
	}

    //--------------------------------------------------------------------------------------
    LoadStatus Device::createResourceTexture(const TextureDesc & _texDesc, const core::string & _name, const void * _initData, Texture *& _texture)
    {
        VG_ASSERT(Kernel::getScheduler()->IsLoadingThread());

        UploadBuffer * streamingBuffer = getStreamingUploadBuffer();

        size_t size, alignment;
        Texture::getRequiredUploadSizeAndAlignment(_texDesc, size, alignment);

        if (streamingBuffer->isAvailable(size, alignment))
        {
            auto requiredUploadSize = streamingBuffer->getAlignedSize(size, alignment);
            streamingBuffer->setNextAllocSize(requiredUploadSize);
            _texture = createTexture(_texDesc, _name, _initData);

            return LoadStatus::Success;
        }
        else
        {
            VG_WARNING("[Resource] Could not upload Texture \"%s\" this frame because a size of %u bytes with alignment of %u in upload buffer was necessary", _name.c_str(), size, alignment);
            return LoadStatus::CannotUploadData;
        }        
    }

    //--------------------------------------------------------------------------------------
    //Texture * Device::createTextureFromFile(const core::string & _path, ReservedSlot _reservedSlot)
    //{
    //    VG_PROFILE_CPU("createTextureFromFile");
    //    TextureDesc texDesc;
    //    core::vector<u8> texData;
    //
    //    TextureImporterSettings settings;
    //    settings.m_mipLevelCount = (TextureImporterMip)1;
    //    settings.m_sRGB = false;
    //
    //    if (m_textureImporter->importTextureData(_path, texDesc, texData, &settings))
    //    {
    //        Texture * tex = createTexture(texDesc, _path, texData.data(), _reservedSlot);
    //        return tex;
    //    }
    //    else
    //    {
    //        VG_ERROR("[Device] Failed to create texture from \"%s\"", _path.c_str());
    //    }
    //    return nullptr;
    //}

    //--------------------------------------------------------------------------------------
    Buffer * Device::createBuffer(const BufferDesc & _bufDesc, const core::string & _name, const void * _initData, ReservedSlot _reservedSlot, CommandList * _cmdList)
    {
        VG_PROFILE_CPU("createBuffer");
        return new Buffer(_bufDesc, _name, _initData, _reservedSlot, _cmdList);
    }

    //--------------------------------------------------------------------------------------
    LoadStatus Device::createResourceBuffer(const BufferDesc & _bufDesc, const core::string & _name, const void * _initData, Buffer *& _buffer)
    {
        VG_ASSERT(Kernel::getScheduler()->IsLoadingThread());

        UploadBuffer * streamingBuffer = getStreamingUploadBuffer();

        size_t size, alignment;
        Buffer::getRequiredUploadSizeAndAlignment(_bufDesc, size, alignment);

        if (streamingBuffer->isAvailable(size, alignment))
        {
            auto requiredUploadSize = streamingBuffer->getAlignedSize(size, alignment);
            getStreamingUploadBuffer()->setNextAllocSize(requiredUploadSize);
            _buffer = createBuffer(_bufDesc, _name, _initData);

            return LoadStatus::Success;
        }
        else
        {
            VG_WARNING("[Resource] Could not upload Buffer \"%s\" this frame because a size of %u bytes with alignment of %u in upload buffer was necessary", _name.c_str(), size, alignment);
            return LoadStatus::CannotUploadData;
        }
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
            super::applyVSync(mode);
        }
    }

    //--------------------------------------------------------------------------------------
    VSync Device::getVSync() const
    {
        return m_VSync;
    }

    //--------------------------------------------------------------------------------------
    void Device::setHDR(HDR _mode)
    {
        if (!m_caps.hdr.isSupported(_mode))
        {
            VG_WARNING("[Device] %s is not supported", asString(_mode).c_str());
            _mode = HDR::None;
        }

        if (_mode != m_HDRModeRequested)
        {
            m_HDRModeRequested = _mode;
            super::applyHDR(m_HDRModeRequested);
        }
    }
    //--------------------------------------------------------------------------------------
    HDR Device::getHDR() const
    {
        return m_HDRMode;
    }

    //--------------------------------------------------------------------------------------
    void Device::waitGPUIdle()
    {
        super::waitGPUIdle();
    }

    //--------------------------------------------------------------------------------------
    core::u64 Device::getAvailableUploadSize() const
    {
        return getStreamingUploadBuffer()->getAvailableUploadSize();
    }

    //--------------------------------------------------------------------------------------
    core::u64 Device::getTotalUploadSize() const
    {
        return getStreamingUploadBuffer()->getTotalSize();
    }

    //--------------------------------------------------------------------------------------
    bool Device::setupRenderJobs(core::uint _maxCount, gfx::RenderJobsPolicy _policy, core::uint _renderJobsTotalBufferSize, core::uint _renderJobsWorkerMinBufferSize)
    {
        if (m_maxRenderJobCount != _maxCount || m_renderJobsPolicy != _policy || _renderJobsTotalBufferSize != m_renderJobsTotalBufferSize || _renderJobsWorkerMinBufferSize != m_renderJobsWorkerMinBufferSize)
        {
            if (m_maxRenderJobCount == -1)
                VG_INFO("[Device] Setup render jobs {%s-%u %u/%u MB}", asCString(_policy), _maxCount, _renderJobsWorkerMinBufferSize >> 20, _renderJobsTotalBufferSize >> 20);
            else
                VG_INFO("[Device] Render jobs setup changed from {%s-%u %u/%u MB} to {%s-%u %u/%u MB}", asCString(m_renderJobsPolicy), m_maxRenderJobCount, m_renderJobsWorkerMinBufferSize>>20, m_renderJobsTotalBufferSize>>20,
                                                                                                        asCString(_policy), _maxCount, _renderJobsWorkerMinBufferSize>>20, _renderJobsTotalBufferSize>>20);
            m_maxRenderJobCount = _maxCount;
            m_renderJobsPolicy = _policy;
            m_renderJobsTotalBufferSize = _renderJobsTotalBufferSize;
            m_renderJobsWorkerMinBufferSize = _renderJobsWorkerMinBufferSize;

            m_renderJobsDirty = true;

            return true;
        }

        return false;
    }
}