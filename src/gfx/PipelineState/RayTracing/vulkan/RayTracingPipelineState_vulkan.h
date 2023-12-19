#pragma once

#include "gfx/PipelineState/RayTracing/RayTracingPipelineState.h"

namespace vg::gfx
{
    class RayTracingPipelineState;

    namespace vulkan
    {
        class RayTracingPipelineState : public base::RayTracingPipelineState
        {
            using super = base::RayTracingPipelineState;

        public:
            RayTracingPipelineState(const RayTracingPipelineStateKey & _key);
            ~RayTracingPipelineState();

            //VkPipeline & getVulkanComputePipeline() { return m_vkComputePipeline; }
            //
            static gfx::RayTracingPipelineState * createRayTracingPipelineState(const RayTracingPipelineStateKey & _rayTracingKey);

        private:
            //VG_INLINE void setVulkanComputePipelineState(const VkPipelineCache & _vkPipelineCache, const VkPipeline & _vkPipeline);
            //static bool createVulkanComputePipelineState(const ComputePipelineStateKey & _computeKey, VkPipelineCache & _vkPipelineCache, VkPipeline & _vkPipeline);

        private:
            //VkPipelineCache m_vkComputePipelineCache;
            //VkPipeline      m_vkComputePipeline;
        };
    }
}