#include "graphics/driver/Precomp.h"
#include "Device.h"
#include "graphics/driver/CommandQueue/CommandQueue.h"
#include "graphics/driver/CommandPool/CommandPool.h"
#include "graphics/driver/CommandList/CommandList.h"
#include "graphics/driver/Texture/Texture.h"

#include VG_GRAPHICSAPI_IMPLEMENTATION(Device)

using namespace vg::graphics;

namespace vg::graphics::driver
{
	using namespace vg::core;

	namespace base
	{
		//--------------------------------------------------------------------------------------
		Device::Device()
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
		}

		//--------------------------------------------------------------------------------------
		void Device::deinit()
		{

		}

		//--------------------------------------------------------------------------------------
		void Device::beginFrame()
		{

		}

		//--------------------------------------------------------------------------------------
		void Device::endFrame()
		{
			// Take the next back buffer from our chain
			m_currentFrameIndex = (m_currentFrameIndex + 1) % max_frame_latency;
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
		vg::graphics::driver::CommandQueue * Device::createCommandQueue(CommandQueueType _type)
		{
			auto & container = m_commandQueues[asInteger(CommandQueueType::Graphics)];
			auto * queue = new graphics::driver::CommandQueue(_type, (uint)container.size());

			container.push_back(queue);

			return queue;
		}

		//--------------------------------------------------------------------------------------
		core::vector<graphics::driver::CommandQueue*> & Device::getCommandQueues(CommandQueueType _type)
		{
			VG_ASSERT(isEnumValue(_type));
			return m_commandQueues[asInteger(_type)];
		}

		//--------------------------------------------------------------------------------------
		core::vector<driver::CommandList*> & Device::getCommandLists(CommandListType _type)
		{
			VG_ASSERT(isEnumValue(_type));
			return getCurrentFrameContext().commandLists[asInteger(_type)];
		}

		//--------------------------------------------------------------------------------------
		void Device::createFrameContext(core::uint _frame, void * _surface)
		{
			auto & context = getFrameContext(_frame);

			// Create backbuffer texture
			{
				TextureDesc backbufferTexDesc
				(
					TextureType::Texture2D,
					PixelFormat::R8G8B8A8,
					TextureFlags::RenderTarget | TextureFlags::Backbuffer | TextureFlags::sRGB,
					getDeviceParams().resolution.x,
					getDeviceParams().resolution.y
				);

				char backbufferName[13];
				sprintf_s(backbufferName, "Backbuffer#%u", _frame);

				m_frameContext[_frame].backbuffer = getDevice()->createTexture(backbufferTexDesc, backbufferName, _surface);
			}

			// Create command pools
			{
				auto & cmdPools = context.commandPools;
				const auto index = (uint)cmdPools.size();
				cmdPools.push_back(new driver::CommandPool(_frame, index));
			}

			// Create command lists
			{
				// Graphics
				{
					auto & cmdPool = context.commandPools[0];
					auto & cmdLists = context.commandLists[asInteger(CommandListType::Graphics)];
					const auto index = (uint)cmdLists.size();
					cmdLists.push_back(new driver::CommandList(CommandListType::Graphics, cmdPool, _frame, index));
				}
			}
		}

		//--------------------------------------------------------------------------------------
		void Device::destroyFrameContext(core::uint _frame)
		{
			auto & pools = m_frameContext[_frame].commandPools;
			for (auto & pool : pools)
				VG_SAFE_RELEASE(pool);
			pools.clear();

			for (auto type : enumValues<CommandListType>())
			{
				auto & cmdLists = m_frameContext[_frame].commandLists[asInteger(type)];
				for (auto & cmdList : cmdLists)
					VG_SAFE_RELEASE(cmdList);
				cmdLists.clear();
			}

			VG_SAFE_RELEASE(m_frameContext[_frame].backbuffer);
		}

		//--------------------------------------------------------------------------------------
		FrameContext & Device::getFrameContext(uint _frame)
		{
			VG_ASSERT(_frame < countof(m_frameContext));
			return m_frameContext[_frame];
		}

		//--------------------------------------------------------------------------------------
		FrameContext & Device::getCurrentFrameContext()
		{
			return getFrameContext(m_currentFrameIndex);
		}

		//--------------------------------------------------------------------------------------
		void Device::destroyCommandQueues()
		{
			for (const auto queueType : enumValues<CommandQueueType>())
			{
				auto & queues = m_commandQueues[asInteger(queueType)];
				for (auto * queue : queues)
					VG_SAFE_RELEASE(queue);
				queues.clear();
			}
		}

		//--------------------------------------------------------------------------------------
		driver::Texture * Device::getBackbuffer()
		{
			return getCurrentFrameContext().backbuffer;
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
		Super::beginFrame();
	}
	
	//--------------------------------------------------------------------------------------
	void Device::endFrame()
	{
		Super::endFrame();
	}

	//--------------------------------------------------------------------------------------
	Texture * Device::createTexture(const TextureDesc & _texDesc, const core::string & _name, void * _initData)
	{
		return new Texture(_texDesc, _name, _initData);
	}
}