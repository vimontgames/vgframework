#pragma once

#include "core/Types/Types.h"
#include "graphics/driver/IDevice.h"
#include "graphics/driver/Device/Device_consts.h"
#include "graphics/driver/CommandQueue/CommandQueue_consts.h"
#include "graphics/driver/CommandList/CommandList_consts.h"

namespace vg::graphics::driver
{
	enum class Severity : core::u8
	{
		Error = 0,
		Warning,
		Info,
		Verbose
	};

	class Device;
	class CommandQueue;
	class CommandPool;
	class CommandList;
	class TextureDesc;
	class Texture;
    class MemoryAllocator;

    enum class PixelFormat : core::u8;

	struct FrameContext
	{
		driver::Texture *					backbuffer = nullptr;
		core::vector<driver::CommandPool*>	commandPools;
		core::vector<driver::CommandList*>	commandLists[core::enumCount<CommandListType>()];
	};

	namespace base
	{
		class Device : public graphics::driver::IDevice
		{
		public:
															Device					();
															~Device					();

			void											init					(const DeviceParams & _params);
			void											deinit					();

			void											beginFrame				();
			void											endFrame				();

			driver::Device *								getDevice				();
			const DeviceParams &							getDeviceParams			() const;

			driver::CommandQueue *							createCommandQueue		(CommandQueueType _type);
			core::vector<driver::CommandQueue*> &			getCommandQueues		(CommandQueueType _type);
			void											destroyCommandQueues	();
			
			void											createFrameContext		(core::uint _frame, void * _backbuffer);
			void											destroyFrameContext		(core::uint _frame);
			FrameContext &									getFrameContext			(core::uint _frame);
			FrameContext &									getCurrentFrameContext	();

			core::vector<driver::CommandList*> &			getCommandLists			(CommandListType _type);
			driver::Texture *								getBackbuffer			();

		protected:
			core::vector<driver::CommandQueue*>				m_commandQueues[core::enumCount<CommandQueueType>()];
			FrameContext									m_frameContext[max_frame_latency];
			core::uint										m_currentFrameIndex = 0;
            driver::PixelFormat                             m_backbufferFormat;
            MemoryAllocator *                               m_memoryAllocator = nullptr;

		private:
			DeviceParams 									m_deviceParams;
		};
	}
}

#include VG_GRAPHICSAPI_HEADER(Device)

namespace vg::graphics::driver
{
	class Device : public core::Singleton_With_Inheritance<Device, VG_GRAPHICSAPI::Device>
	{
		using Super = VG_GRAPHICSAPI::Device;

	public:
		void		init			(const DeviceParams & _params);
		void		deinit			();

		void		beginFrame		();
		void		endFrame		();

		Texture *	createTexture	(const TextureDesc & _texDesc, const core::string & _name, void * _initData = nullptr);

	private:
	};
}