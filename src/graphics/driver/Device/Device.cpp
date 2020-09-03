#include "graphics/driver/Precomp.h"
#include "Device.h"
#include "graphics/driver/CommandQueue/CommandQueue.h"
#include "graphics/driver/CommandPool/CommandPool.h"
#include "graphics/driver/CommandList/CommandList.h"
#include "graphics/driver/Texture/Texture.h"
#include "graphics/driver/Memory/MemoryAllocator.h"
#include "graphics/driver/RootSignature/RootSignature.h"

using namespace vg::core;
using namespace vg::graphics;

#include VG_GRAPHICSAPI_IMPLEMENTATION(Device)

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
		void Device::createFrameContext(core::uint _frameContextIndex, void * _surface)
		{
			auto & context = getFrameContext(_frameContextIndex);

			// Create backbuffer texture
			{
				TextureDesc backbufferTexDesc
				(
					TextureType::Texture2D,
					m_backbufferFormat,
					TextureFlags::RenderTarget | TextureFlags::Backbuffer | TextureFlags::sRGB,
					getDeviceParams().resolution.x,
					getDeviceParams().resolution.y
				);

				char backbufferName[13];
				sprintf_s(backbufferName, "Backbuffer#%u", _frameContextIndex);

				m_frameContext[_frameContextIndex].backbuffer = getDevice()->createTexture(backbufferTexDesc, backbufferName, _surface);
			}

			// Create command pools
			{
				auto & cmdPools = context.commandPools;
				const auto index = (uint)cmdPools.size();
				cmdPools.push_back(new driver::CommandPool(_frameContextIndex, index));
			}

			// Create command lists
			{
				// Graphics
				{
					auto & cmdPool = context.commandPools[0];
					auto & cmdLists = context.commandLists[asInteger(CommandListType::Graphics)];
					const auto index = (uint)cmdLists.size();
					cmdLists.push_back(new driver::CommandList(CommandListType::Graphics, cmdPool, _frameContextIndex, index));
				}
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

            if (frameContext.m_objectsToRelease.size())
                VG_DEBUGPRINT("Release %u object(s) async [%u]\n", frameContext.m_objectsToRelease.size(), getFrameContextIndex());

            for (Object * obj : frameContext.m_objectsToRelease)
                VG_SAFE_RELEASE(obj);
            frameContext.m_objectsToRelease.clear();
        }

		//--------------------------------------------------------------------------------------
		void Device::destroyCommandQueues()
		{
			for (uint queueType = 0; queueType < enumCount<CommandQueueType>(); ++queueType)
			{
				auto & queues = m_commandQueues[queueType];
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
        VG_DEBUGPRINT("beginFrame #%u\n{\n", getFrameCounter());
		Super::beginFrame();

        // It is safe now to release frame (n-max_frame_latency+1) resources as we are now sure they are not in use by the GPU
        flushReleaseAsync();

        VG_DEBUGPRINT("[...]\n");
	}
	
	//--------------------------------------------------------------------------------------
	void Device::endFrame()
	{
		Super::endFrame();
        VG_DEBUGPRINT("}\nendFrame #%u\n", getFrameCounter());
	}

	//--------------------------------------------------------------------------------------
	Texture * Device::createTexture(const TextureDesc & _texDesc, const core::string & _name, void * _initData)
	{
		return new Texture(_texDesc, _name, _initData);
	}

    //--------------------------------------------------------------------------------------
    RootSignatureHandle Device::addRootSignature(const RootSignatureDesc & _desc)
    {
        return m_rootSignaturesTable.add(_desc);

        /*
        // find existing
        {
            uint index = 0;
            for (auto & pair : m_rootSignatures)
            {
                if (pair.first == _desc)
                {
                    VG_ASSERT(pair.second);
                    pair.second->addRef();
                    return index;
                }
                ++index;
            }
        }

        // reuse empty slot
        {
            uint index = 0;
            for (auto & pair : m_rootSignatures)
            {
                if (nullptr == pair.second)
                {
                    pair.first = _desc;
                    pair.second = new RootSignature(_desc);
                    return index;
                }
                ++index;
            }
        }

        // add
        {
            uint index = (uint)m_rootSignatures.size();
            m_rootSignatures.push_back(std::pair<RootSignatureDesc, RootSignature*>(_desc, new RootSignature(_desc)));
            return index;
        }*/
    }

    //--------------------------------------------------------------------------------------
    uint Device::removeRootSignature(RootSignatureHandle & _handle)
    {
        return m_rootSignaturesTable.remove(_handle);
    }
}