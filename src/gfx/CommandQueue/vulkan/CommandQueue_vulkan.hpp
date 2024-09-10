#include "gfx/CommandList/CommandList.h"

namespace vg::gfx::vulkan
{
	//--------------------------------------------------------------------------------------
	CommandQueue::CommandQueue(CommandQueueType _type) :
		base::CommandQueue(_type)
	{
		auto * device = gfx::Device::get();
		const auto familyIndex = device->getVulkanCommandQueueFamilyIndex(_type);
		vkGetDeviceQueue(device->getVulkanDevice(), familyIndex, 0, &m_vkCommandQueue);

		m_timestampPeriod = device->getVulkanPhysicalDeviceProperties().limits.timestampPeriod;
        
        VkQueryPoolCreateInfo queryPoolCreateInfo;
        queryPoolCreateInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
        queryPoolCreateInfo.pNext = 0;
        queryPoolCreateInfo.flags = 0;
        queryPoolCreateInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
        queryPoolCreateInfo.queryCount = MaxQueriesCount;

		vkCreateQueryPool(device->getVulkanDevice(), &queryPoolCreateInfo, nullptr, &m_vkQueryPool);
		vkResetQueryPool(device->getVulkanDevice(), m_vkQueryPool, 0, MaxQueriesCount);
	}

	//--------------------------------------------------------------------------------------
	CommandQueue::~CommandQueue()
	{
		auto * device = gfx::Device::get();
		vkDestroyQueryPool(device->getVulkanDevice(), m_vkQueryPool, nullptr);
	}

    //--------------------------------------------------------------------------------------
	void CommandQueue::beginFrame(gfx::CommandList * _cmdList)
	{
        vkCmdWriteTimestamp(_cmdList->getVulkanCommandBuffer(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, m_vkQueryPool, m_queryIndex + 0);
	}

    //--------------------------------------------------------------------------------------
	void CommandQueue::endFrame(gfx::CommandList * _cmdList)
	{
		vkCmdWriteTimestamp(_cmdList->getVulkanCommandBuffer(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, m_vkQueryPool, m_queryIndex + 1);

		// read N-2
		auto * device = gfx::Device::get();
		auto readQueryIndex = (m_queryIndex + 2) % MaxQueriesCount;
		core::u64 results[2];
		vkGetQueryPoolResults(device->getVulkanDevice(), m_vkQueryPool, readQueryIndex, 2, sizeof(core::u64) * 2, &results, sizeof(core::u64), VK_QUERY_RESULT_64_BIT);
		vkResetQueryPool(device->getVulkanDevice(), m_vkQueryPool, readQueryIndex, 2);
		core::u64 elapsed = results[1] - results[0];
		double enlapsedTime = (double)(elapsed * m_timestampPeriod) * 0.000001;
		VG_DEBUGPRINT("GPU frame time = %.3f ms\n", enlapsedTime);

		m_queryIndex = (m_queryIndex + 2) % MaxQueriesCount;
	}
}