#include "gfx/PipelineState/Graphic/GraphicPipelineState.h"

#if !VG_ENABLE_INLINE
#include "ComputePipelineState_vulkan.inl"
#endif

namespace vg::gfx::vulkan
{
    //--------------------------------------------------------------------------------------
    ComputePipelineState::ComputePipelineState(const ComputePipelineStateKey & _computeKey) :
        super::ComputePipelineState(_computeKey)
    {
    }

    //--------------------------------------------------------------------------------------
    gfx::ComputePipelineState * ComputePipelineState::createComputePipelineState(const ComputePipelineStateKey & _computeKey)
    {
        VkPipelineCache vkPipelineCache = {};
        VkPipeline vkPipeline = {};

        if (createVulkanComputePipelineState(_computeKey, vkPipelineCache, vkPipeline))
        {
            gfx::ComputePipelineState * pso = new gfx::ComputePipelineState(_computeKey);
            pso->setVulkanComputePipelineState(vkPipelineCache, vkPipeline);
            return pso;
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    VkShaderStageFlagBits ComputePipelineState::getVulkanShaderStage(ComputeStage _stage)
    {
        switch (_stage)
        {
            default:
                VG_ASSERT(false, "Unhandled ComputeStage \"%s\" (%u)", asString(_stage).c_str(), _stage);
                return VkShaderStageFlagBits(0);

            case ComputeStage::Compute:
                return VK_SHADER_STAGE_COMPUTE_BIT;
        }
    }

    //--------------------------------------------------------------------------------------
    bool ComputePipelineState::createVulkanComputePipelineState(const ComputePipelineStateKey & _computeKey, VkPipelineCache & _vkPipelineCache, VkPipeline & _vkPipeline)
    {
        auto * device = gfx::Device::get();
        auto & vkDevice = device->getVulkanDevice();

        VkPipelineCache vkPipelineCache = {};

        VkPipelineCacheCreateInfo vkPipelineCacheDesc = {};
        vkPipelineCacheDesc.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        VG_ASSERT_VULKAN(vkCreatePipelineCache(vkDevice, &vkPipelineCacheDesc, nullptr, &vkPipelineCache));

        VkComputePipelineCreateInfo vkPipelineDesc = {};

        // PipelineLayout
        vkPipelineDesc.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        vkPipelineDesc.layout = device->getRootSignature(_computeKey.m_computeRootSignature)->getVulkanPipelineLayout();

        // Shaders
        vector<VkPipelineShaderStageCreateInfo> vkShaderStages;

        auto * sm = ShaderManager::get();
        auto * desc = sm->getHLSLDescriptor(_computeKey.m_computeShaderKey.file);

        if (desc)
        {
            ComputePrograms computePrograms;

            if (!desc->getComputePrograms(API::Vulkan, _computeKey.m_computeShaderKey, computePrograms))
                return false;

            VkPipelineShaderStageCreateInfo vkShaderStage = {};
                
            vkShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vkShaderStage.stage = getVulkanShaderStage(ComputeStage::Compute);
            vkShaderStage.module = computePrograms.m_shaders[asInteger(ComputeStage::Compute)]->getVulkanBytecode();
            vkShaderStage.pName = computePrograms.m_shaders[asInteger(ComputeStage::Compute)]->getName().c_str();
                
            vkShaderStages.push_back(vkShaderStage);
        }

        vkPipelineDesc.stage = vkShaderStages[0];

        VkPipeline vkPipeline = {};
        VG_ASSERT_VULKAN(vkCreateComputePipelines(vkDevice, vkPipelineCache, 1, &vkPipelineDesc, nullptr, &vkPipeline));

        _vkPipelineCache = vkPipelineCache;
        _vkPipeline = vkPipeline;

        return true;
    }

    //--------------------------------------------------------------------------------------
    ComputePipelineState::~ComputePipelineState()
    {
        auto & vkDevice = gfx::Device::get()->getVulkanDevice();

        vkDestroyPipeline(vkDevice, m_vkComputePipeline, nullptr);
        vkDestroyPipelineCache(vkDevice, m_vkComputePipelineCache, nullptr);
    }    
}