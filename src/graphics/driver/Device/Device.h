#pragma once

#include "core/Types/Types.h"
#include "graphics/driver/IDevice.h"
#include "graphics/driver/Device/Device_consts.h"
#include "graphics/driver/BindlessTable/BindlessTable_consts.h"
#include "graphics/driver/CommandQueue/CommandQueue_consts.h"
#include "graphics/driver/CommandList/CommandList_consts.h"
#include "graphics/driver/RootSignature/RootSignature_consts.h"
#include "graphics/driver/Device/DeviceCaps.h"

//#define VG_DBG_CPUGPUSYNC 1

namespace vg::graphics::driver
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

    enum class PixelFormat : core::u8;

    struct BufferContext
    {
        driver::Texture *					            backbuffer = nullptr;
    };

	struct FrameContext
	{
		core::vector<driver::CommandPool*>	            commandPools;
		core::vector<driver::CommandList*>	            commandLists[core::enumCount<CommandListType>()];
        core::vector<core::Object*>                     m_objectsToRelease;
        driver::Buffer *                                m_uploadBuffer = nullptr;
        core::u8 *                                      m_uploadBegin = nullptr;
        core::u8 *                                      m_uploadCur = nullptr;
        core::vector<core::pair<Texture*, core::u32>>   m_texturesToUpload;
        core::vector<core::pair<Buffer*, core::u32>>    m_buffersToUpload;
        UINT64                                          mFrameFenceId;
	};

	namespace base
	{
		class Device : public graphics::driver::IDevice
		{
		public:
															Device					    ();
															~Device					    ();

			void											init					    (const DeviceParams & _params);
			void											deinit					    ();

			void											beginFrame				    ();
			void											endFrame				    ();

			driver::Device *								getDevice				    ();
			const DeviceParams &							getDeviceParams			    () const;

			driver::CommandQueue *							createCommandQueue		    (CommandQueueType _type);
			driver::CommandQueue * 			                getCommandQueue		        (CommandQueueType _type);
			void											destroyCommandQueues	    ();
			
            core::u64                                       getFrameCounter() const;

			void											createFrameContext		    (core::uint _frameContextIndex);
			void											destroyFrameContext		    (core::uint _frameContextIndex);
			core::uint                                      getFrameContextIndex        () const;
            FrameContext &									getCurrentFrameContext      ();

            void											createBackbuffer            (core::uint _backbufferIndex, void * _backbuffer);
            void											destroyBackbuffer           (core::uint _backbufferIndex);
            core::uint                                      getBackbufferIndex          () const;
            BufferContext &									getCurrentBackbuffer        ();

			core::vector<driver::CommandList*> &			getCommandLists			    (CommandListType _type);
			driver::Texture *								getBackbuffer			    ();

            void                                            releaseAsync                (core::Object * _object);
            void                                            flushReleaseAsync           ();

            void                                            upload                      (driver::Texture * _dst, core::u32 _from);
            void                                            upload                      (driver::Buffer * _dst, core::u32 _from);

            BindlessTable *                                 getBindlessTable            () const;
            driver::PixelFormat                             getBackbufferFormat         () const { return m_backbufferFormat; }

		//protected:
            DeviceCaps                                      m_caps;
			driver::CommandQueue*				            m_commandQueue[core::enumCount<CommandQueueType>()];
			FrameContext									m_frameContext[max_frame_latency];
            BufferContext                                   m_bufferContext[max_backbuffer_count];
			core::u64										m_frameCounter = 0;
            driver::PixelFormat                             m_backbufferFormat;
            MemoryAllocator *                               m_memoryAllocator = nullptr;
			DeviceParams 									m_deviceParams;
            ShaderManager *                                 m_shaderManager = nullptr;
            RootSignatureTable                              m_rootSignaturesTable;
            driver::BindlessTable *                         m_bindlessTable = nullptr;

            core::u8                                        m_currentFrameIndex;        // current frame being rendered
            core::u8                                        m_nextFrameIndex;
            core::u8                                        m_currentBackbufferIndex;   // current backbuffer being used
		};
	}
}

#include VG_GFXAPI_HEADER(Device)

namespace vg::graphics::driver
{
    class TextureImporter;

	class Device : public VG_GFXAPI::Device, public core::Singleton<Device>
	{
		using super = VG_GFXAPI::Device;

	public:
		void		        init			    (const DeviceParams & _params);
		void		        deinit			    ();

        void                resize              (core::uint _width, core::uint _height);

		void		        beginFrame		    ();
		void		        endFrame		    ();

		Texture *	        createTexture	    (const TextureDesc & _texDesc, const core::string & _name, void * _initData = nullptr, ReservedSlot _reservedSlot = ReservedSlot::None);
        Texture *	        createTexture       (const core::string & _path, ReservedSlot _reservedSlot = ReservedSlot::None);
        Buffer *            createBuffer        (const BufferDesc & _bufDesc, const core::string & _name, void * _initData = nullptr, ReservedSlot _reservedSlot = ReservedSlot::None);

        RootSignatureHandle addRootSignature    (const RootSignatureDesc & _desc);
        RootSignature *     getRootSignature    (const RootSignatureHandle & _handle);
        core::uint          removeRootSignature (RootSignatureHandle & _handle);

        void                flushTextureUploads ();

        bool                isMinimized         ();

	private:
        TextureImporter *   m_textureImporter = nullptr;
	};
}