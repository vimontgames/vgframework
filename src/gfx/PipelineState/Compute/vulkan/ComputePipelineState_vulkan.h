#pragma once

#include "gfx/PipelineState/Compute/ComputePipelineState.h"

namespace vg::gfx
{
    class ComputePipelineState;
    class HLSLDesc;

    namespace vulkan
    {
        class ComputePipelineState : public base::ComputePipelineState
        {
            using super = base::ComputePipelineState;

        public:
            ComputePipelineState(const ComputePipelineStateKey & _computeKey);
            ~ComputePipelineState();

            VkPipeline & getVulkanComputePipeline() { return m_vkComputePipeline; }

            static gfx::ComputePipelineState * createComputePipelineState(const ComputePipelineStateKey & _computeKey);

        private:
            VG_INLINE void setVulkanComputePipelineState(const VkPipelineCache & _vkPipelineCache, const VkPipeline & _vkPipeline);
            static bool createVulkanComputePipelineState(const ComputePipelineStateKey & _computeKey, VkPipelineCache & _vkPipelineCache, VkPipeline & _vkPipeline);

        private:
            VkPipelineCache m_vkComputePipelineCache;
            VkPipeline      m_vkComputePipeline;
        };
    }
}

#if VG_ENABLE_INLINE
#include "ComputePipelineState_vulkan.inl"
#endif