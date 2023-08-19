#pragma once

#include "gfx/PipelineState/GraphicPipelineState.h"

namespace vg::gfx
{
    class GraphicPipelineState;
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

            static gfx::GraphicPipelineState * createGraphicPipelineState(const GraphicPipelineStateKey & _key);
            static VkShaderStageFlagBits getVulkanShaderStage(ShaderStage _stage);

        private:
            VG_INLINE static VkPrimitiveTopology getVulkanPrimitiveTopology(PrimitiveTopology _topology);
            static bool addVulkanShader(core::vector<VkPipelineShaderStageCreateInfo> & _vkStages, HLSLDesc & _hlsl, ShaderStage _stage, ShaderKey::EntryPoint _entryPointIndex, ShaderKey::Flags _flags);

            VG_INLINE void setVulkanGraphicPipelineState(const VkPipelineCache & _vkPipelineCache, const VkPipeline & _vkPipeline);
            static bool createVulkanGraphicPipelineState(const GraphicPipelineStateKey & _key, VkPipelineCache & _vkPipelineCache, VkPipeline & _vkPipeline);

        private:
            VkPipelineCache m_vkPipelineCache;
            VkPipeline      m_vkPipeline;
        };
    }
}

#if VG_ENABLE_INLINE
#include "GraphicPipelineState_vulkan.inl"
#endif