#pragma once

#include "core/Types/Types.h"
#include "graphics/driver/IDevice.h"
#include "graphics/driver/Device/Device_consts.h"
#include "graphics/driver/CommandQueue/CommandQueue_consts.h"
#include "graphics/driver/CommandList/CommandList_consts.h"
#include "graphics/driver/Device/DeviceCaps.h"
#include "graphics/driver/RootSignature/RootSignature_consts.h"

//#define VG_DBG_CPUGPUSYNC 1

namespace vg::graphics::driver
{
	class Device;
	class CommandQueue;
	class CommandPool;
	class CommandList;
	class TextureDesc;
	class Texture;
    class MemoryAllocator;
    class RootSignature;
    class RootSignatureDesc;
    class ShaderManager;

    enum class PixelFormat : core::u8;

	struct FrameContext
	{
		driver::Texture *					backbuffer = nullptr;
		core::vector<driver::CommandPool*>	commandPools;
		core::vector<driver::CommandList*>	commandLists[core::enumCount<CommandListType>()];
        core::vector<core::Object*>         m_objectsToRelease;
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
			core::vector<driver::CommandQueue*> &			getCommandQueues		    (CommandQueueType _type);
			void											destroyCommandQueues	    ();
			
			void											createFrameContext		    (core::uint _frameContextIndex, void * _backbuffer);
			void											destroyFrameContext		    (core::uint _frameContextIndex);
			FrameContext &									getFrameContext			    (core::uint _frameContextIndex);
            core::u64                                       getFrameCounter             () const;
			core::uint                                      getFrameContextIndex        () const;
            FrameContext &									getCurrentFrameContext      ();

			core::vector<driver::CommandList*> &			getCommandLists			    (CommandListType _type);
			driver::Texture *								getBackbuffer			    ();

            void                                            releaseAsync                (core::Object * _object);
            void                                            flushReleaseAsync           ();

		protected:
            DeviceCaps                                      m_caps;
			core::vector<driver::CommandQueue*>				m_commandQueues[core::enumCount<CommandQueueType>()];
			FrameContext									m_frameContext[max_frame_latency];
			core::u64										m_frameCounter = 0;
            driver::PixelFormat                             m_backbufferFormat;
            MemoryAllocator *                               m_memoryAllocator = nullptr;
			DeviceParams 									m_deviceParams;
            ShaderManager *                                 m_shaderManager = nullptr;
            RootSignatureTable                              m_rootSignaturesTable;
		};
	}
}

#include VG_GFXAPI_HEADER(Device)

namespace vg::graphics::driver
{
	class Device : public VG_GFXAPI::Device, public core::Singleton<Device>
	{
		using Super = VG_GFXAPI::Device;

	public:
		void		        init			    (const DeviceParams & _params);
		void		        deinit			    ();

		void		        beginFrame		    ();
		void		        endFrame		    ();

		Texture *	        createTexture	    (const TextureDesc & _texDesc, const core::string & _name, void * _initData = nullptr);

        RootSignatureHandle addRootSignature    (const RootSignatureDesc & _desc);
        RootSignature *     getRootSignature    (const RootSignatureHandle & _handle);
        core::uint          removeRootSignature (RootSignatureHandle & _handle);

	private:
	};
}