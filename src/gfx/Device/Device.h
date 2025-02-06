#pragma once

#include "core/Types/Types.h"
#include "gfx/IDevice.h"
#include "gfx/Device/Device_consts.h"
#include "gfx/BindlessTable/BindlessTable_consts.h"
#include "gfx/CommandQueue/CommandQueue_consts.h"
#include "gfx/CommandList/CommandList_consts.h"
#include "gfx/RootSignature/RootSignature_consts.h"
#include "gfx/Device/DeviceCaps.h"
#include "gfx/Shader/ShaderKey.h"
#include "gfx/FrameGraph/FrameGraph_consts.h"

//#define VG_DBG_CPUGPUSYNC 1

namespace vg::core
{
    using Ticks = i64;
}

namespace vg::gfx
{
	class Device;
	class CommandQueue;
	class CommandPool;
	class CommandList;
	class TextureDesc;
	class Texture;
    class BufferDesc;
    class Buffer;
    class MemoryAllocator;
    class RootSignature;
    class RootSignatureDesc;
    class ShaderManager;
    class BindlessTable;
    class UploadBuffer;

    enum class PixelFormat : core::u8;

    struct BufferContext
    {
        gfx::Texture *                      backbuffer = nullptr;
    };

	struct FrameContext
	{
        FrameContext()
        {
            for (uint i = 0; i < core::enumCount<CommandListType>(); ++i)
                m_executeCommandListCount[i] = 0;

            m_executeCommandListCount[core::asInteger(CommandListType::Graphics)] = 1;
            m_frameFenceId = -1;
        }

		core::vector<gfx::CommandPool*>     m_commandPools;
		core::vector<gfx::CommandList*>     m_commandLists[core::enumCount<CommandListType>()];
        core::u32                           m_executeCommandListCount[core::enumCount<CommandListType>()];
        core::u64                           m_frameFenceId;
	};

	namespace base
	{
		class Device : public gfx::IDevice
		{
		public:
															Device					    ();
															~Device					    ();

			void											init					    (const DeviceParams & _params);
			void											deinit					    ();

			void											beginFrame				    ();
			void											endFrame				    ();

			gfx::Device *								    getDevice				    ();
			const DeviceParams &							getDeviceParams			    () const;

			gfx::CommandQueue *							    createCommandQueue		    (CommandQueueType _type);
			gfx::CommandQueue * 			                getCommandQueue		        (CommandQueueType _type);
			void											destroyCommandQueues	    ();
			
            core::u64                                       getFrameCounter             () const;

			void											createFrameContext		    (core::uint _frameContextIndex);
            void                                            updateFrameContext          ();
			void											destroyFrameContext		    (core::uint _frameContextIndex);
			core::uint                                      getFrameContextIndex        () const;
            FrameContext &									getCurrentFrameContext      ();

            void                                            createUploadBuffer          ();
            void                                            updateUploadBuffers         ();
            void                                            destroyUploadBuffers        ();
            UploadBuffer *                                  getUploadBuffer             (core::uint _index);

            void											createBackbuffer            (core::uint _backbufferIndex, void * _backbuffer);
            void											destroyBackbuffer           (core::uint _backbufferIndex);
            core::uint                                      getBackbufferIndex          () const;
            BufferContext &									getCurrentBackbuffer        ();

			core::vector<gfx::CommandList*> &			    getCommandLists			    (CommandListType _type);
            void                                            setExecuteCommandListCount  (CommandListType _type, core::uint _count);
			gfx::Texture *								    getBackbuffer			    ();

            BindlessTable *                                 getBindlessTable            () const;
            gfx::PixelFormat                                getBackbufferFormat         () const { return m_backbufferFormat; }
            gfx::ShaderManager *                            getShaderManager            () { return m_shaderManager;}

            const gfx::DeviceCaps &                         getDeviceCaps               () const { return m_caps; }

            PixelFormat                                     getHDRBackbufferFormat      (HDR _mode) const;
            ColorSpace                                      getHDRColorSpace            (HDR _mode) const;

            VG_INLINE double                                getGpuFrameTime             () const;
            void                                            setGpuFrameTime             (double _gpuFrameTime);

            VG_INLINE double                                getGpuWaitTime              () const;

            void							                beginCapture                ();
            void							                endCapture                  ();

            void                                            beginWaitGPU                ();
            void                                            endWaitGPU                  ();

		protected:
            gfx::DeviceCaps                                 m_caps;
			gfx::CommandQueue*				                m_commandQueue[core::enumCount<CommandQueueType>()];
			FrameContext									m_frameContext[max_frame_latency];
            core::vector<UploadBuffer *>                    m_uploadBuffers = {};
            BufferContext                                   m_bufferContext[max_backbuffer_count];
			core::u64										m_frameCounter = 0;
            gfx::PixelFormat                                m_backbufferFormat;
            MemoryAllocator *                               m_memoryAllocator = nullptr;
			DeviceParams 									m_deviceParams;
            ShaderManager *                                 m_shaderManager = nullptr;
            RootSignatureTable                              m_rootSignaturesTable;
            gfx::BindlessTable *                            m_bindlessTable = nullptr;
            VSync                                           m_VSync = VSync::VSync_1;
            HDR                                             m_HDRModeRequested = HDR::None;
            HDR                                             m_HDRMode = m_HDRModeRequested;
            ColorSpace                                      m_ColorSpaceRequested = ColorSpace::Rec709;
            ColorSpace                                      m_ColorSpace = m_ColorSpaceRequested;
            bool						                    m_dirtySwapchain = false;
            core::u8                                        m_currentFrameIndex;        // current frame being rendered
            core::u8                                        m_nextFrameIndex;
            core::u8                                        m_currentBackbufferIndex;   // current backbuffer being used
            double                                          m_gpuFrameTime = 0;
            core::Ticks                                     m_beginWaitGPUTicks = 0;
            double                                          m_gpuWaitTime = 0;
            bool                                            m_captureInProgress = false;
            core::uint                                      m_maxRenderJobCount = -1;
            gfx::RenderJobsPolicy                           m_renderJobsPolicy = gfx::RenderJobsPolicy::RecursiveSplit;
            core::u32                                       m_renderJobsTotalBufferSize = 64 * 1024 * 1024;
            core::u32                                       m_renderJobsWorkerMinBufferSize = 4 * 1024 * 1024;
            bool                                            m_renderJobsDirty = true;
		};
	}
}

#ifdef VG_DX12
#include "dx12/Device_dx12.h"
#elif defined(VG_VULKAN)
#include "vulkan/Device_vulkan.h"
#else
#error Undefined GFXAPI
#endif

namespace vg::gfx
{
    class TextureImporter;

	class Device : public VG_GFXAPI::Device, public core::Singleton<Device>
	{
		using super = VG_GFXAPI::Device;

	public:
        const char *            GetClassName                () const final { return "Device"; }

		void		            init			            (const DeviceParams & _params);
		void		            deinit			            ();

        void                    resize                      (core::uint _width, core::uint _height);

		void		            beginFrame		            ();
		void		            endFrame		            ();

		Texture *	            createTexture	            (const TextureDesc & _texDesc, const core::string & _name, const void * _initData = nullptr, ReservedSlot _reservedSlot = ReservedSlot::None);
        Texture *	            createTextureFromFile       (const core::string & _path, ReservedSlot _reservedSlot = ReservedSlot::None);
        Buffer *                createBuffer                (const BufferDesc & _bufDesc, const core::string & _name, const void * _initData = nullptr, ReservedSlot _reservedSlot = ReservedSlot::None);

        RootSignatureHandle     addRootSignature            (const RootSignatureDesc & _desc);
        RootSignature *         getRootSignature            (const RootSignatureHandle & _handle);
        core::uint              removeRootSignature         (RootSignatureHandle & _handle);

        void                    flushUploadBuffer           ();

        bool                    isMinimized                 ();
        void                    resetShaders                (ShaderKey::File _file);

        void                    setVSync                    (VSync mode);
        VSync                   getVSync                    () const;

        void                    setHDR                      (HDR _mode);
        HDR                     getHDR                      () const;

        bool                    setupRenderJobs             (core::uint _maxCount, gfx::RenderJobsPolicy _policy, core::uint _renderJobsTotalBufferSize, core::uint  _renderJobsWorkerMinBufferSize);

        VG_INLINE core::uint    getMaxRenderJobCount        () const;
        VG_INLINE gfx::RenderJobsPolicy getRenderJobsPolicy () const;
        VG_INLINE core::uint    getMaxRenderTotalBufferSize () const;
        VG_INLINE core::uint    getMaxRenderMinBufferSize   () const;

        void                    waitGPUIdle                 ();

	private:
        TextureImporter *       m_textureImporter = nullptr;
	};
}

#if VG_ENABLE_INLINE
#include "Device.inl"
#endif