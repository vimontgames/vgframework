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

//#define VG_DBG_CPUGPUSYNC 1

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
		core::vector<gfx::CommandPool*>     commandPools;
		core::vector<gfx::CommandList*>     commandLists[core::enumCount<CommandListType>()];
        core::vector<core::Object*>         m_objectsToRelease;
        UINT64                              mFrameFenceId;
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

			gfx::Device *								getDevice				    ();
			const DeviceParams &							getDeviceParams			    () const;

			gfx::CommandQueue *							createCommandQueue		    (CommandQueueType _type);
			gfx::CommandQueue * 			                getCommandQueue		        (CommandQueueType _type);
			void											destroyCommandQueues	    ();
			
            core::u64                                       getFrameCounter() const;

			void											createFrameContext		    (core::uint _frameContextIndex);
			void											destroyFrameContext		    (core::uint _frameContextIndex);
			core::uint                                      getFrameContextIndex        () const;
            FrameContext &									getCurrentFrameContext      ();

            void                                            createUploadBuffer          ();
            void                                            destroyUploadBuffer         ();
            UploadBuffer *                                  getUploadBuffer             ();

            void											createBackbuffer            (core::uint _backbufferIndex, void * _backbuffer);
            void											destroyBackbuffer           (core::uint _backbufferIndex);
            core::uint                                      getBackbufferIndex          () const;
            BufferContext &									getCurrentBackbuffer        ();

			core::vector<gfx::CommandList*> &			getCommandLists			    (CommandListType _type);
			gfx::Texture *								getBackbuffer			    ();

            void                                            releaseAsync                (core::Object * _object);
            void                                            flushReleaseAsync           ();

            BindlessTable *                                 getBindlessTable            () const;
            gfx::PixelFormat                             getBackbufferFormat         () const { return m_backbufferFormat; }
            gfx::ShaderManager *                         getShaderManager            () { return m_shaderManager;}

		protected:
            DeviceCaps                                      m_caps;
			gfx::CommandQueue*				            m_commandQueue[core::enumCount<CommandQueueType>()];
			FrameContext									m_frameContext[max_frame_latency];
            UploadBuffer *                                  m_uploadBuffer = nullptr;
            BufferContext                                   m_bufferContext[max_backbuffer_count];
			core::u64										m_frameCounter = 0;
            gfx::PixelFormat                             m_backbufferFormat;
            MemoryAllocator *                               m_memoryAllocator = nullptr;
			DeviceParams 									m_deviceParams;
            ShaderManager *                                 m_shaderManager = nullptr;
            RootSignatureTable                              m_rootSignaturesTable;
            gfx::BindlessTable *                         m_bindlessTable = nullptr;
            VSync                                           m_VSync = VSync::VBL_1;
            core::u8                                        m_currentFrameIndex;        // current frame being rendered
            core::u8                                        m_nextFrameIndex;
            core::u8                                        m_currentBackbufferIndex;   // current backbuffer being used


		};
	}
}

#include VG_GFXAPI_HEADER(Device)

namespace vg::gfx
{
    class TextureImporter;

	class Device : public VG_GFXAPI::Device, public core::Singleton<Device>
	{
		using super = VG_GFXAPI::Device;

	public:
        const char *        getClassName        () const final { return "Device"; }

		void		        init			    (const DeviceParams & _params);
		void		        deinit			    ();

        void                resize              (core::uint _width, core::uint _height);

		void		        beginFrame		    ();
		void		        endFrame		    ();

		Texture *	        createTexture	    (const TextureDesc & _texDesc, const core::string & _name, const void * _initData = nullptr, ReservedSlot _reservedSlot = ReservedSlot::None);
        Texture *	        createTexture       (const core::string & _path, ReservedSlot _reservedSlot = ReservedSlot::None);
        Buffer *            createBuffer        (const BufferDesc & _bufDesc, const core::string & _name, const void * _initData = nullptr, ReservedSlot _reservedSlot = ReservedSlot::None);

        RootSignatureHandle addRootSignature    (const RootSignatureDesc & _desc);
        RootSignature *     getRootSignature    (const RootSignatureHandle & _handle);
        core::uint          removeRootSignature (RootSignatureHandle & _handle);

        void                flushUploadBuffer ();

        bool                isMinimized         ();
        void                resetShaders        (ShaderKey::File _file);

        void                setVSync            (VSync mode);
        VSync               getVSync            () const;

	private:
        TextureImporter *   m_textureImporter = nullptr;
	};
}