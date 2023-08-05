#pragma once

#include "gfx/PipelineState/GraphicPipelineState.h"

namespace vg::gfx
{
    class HLSLDesc;

    namespace vulkan
    {
        class GraphicPipelineState : public base::GraphicPipelineState
        {
            using super = base::GraphicPipelineState;

        public:
            GraphicPipelineState(const GraphicPipelineStateKey & _key);
            ~GraphicPipelineState();

            VkPipeline & getVulkanPipeline() { return m_vkPipeline; }

            static VkShaderStageFlagBits getVulkanShaderStage(ShaderStage _stage);

        private:
            static VkPrimitiveTopology getVulkanPrimitiveTopology(PrimitiveTopology _topology);

            void addVulkanShader(core::vector<VkPipelineShaderStageCreateInfo> & _vkStages, HLSLDesc & _hlsl, ShaderStage _stage, core::uint _index);

        private:
            VkPipelineCache m_vkPipelineCache;
            VkPipeline  m_vkPipeline;
        };
    }
}