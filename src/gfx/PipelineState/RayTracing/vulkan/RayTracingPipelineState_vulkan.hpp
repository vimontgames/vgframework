#include "gfx/PipelineState/RayTracing/RayTracingPipelineState.h"

namespace vg::gfx::vulkan
{
    //--------------------------------------------------------------------------------------
    RayTracingPipelineState::RayTracingPipelineState(const RayTracingPipelineStateKey & _key) :
        super(_key)
    {
    }

    //--------------------------------------------------------------------------------------
    gfx::RayTracingPipelineState * RayTracingPipelineState::createRayTracingPipelineState(const RayTracingPipelineStateKey & _rayTracingKey)
    {
        VG_ASSERT(false);
        //VkPipelineCache vkPipelineCache = {};
        //VkPipeline vkPipeline = {};
        //
        //if (createVulkanComputePipelineState(_computeKey, vkPipelineCache, vkPipeline))
        //{
        //    gfx::ComputePipelineState * pso = new gfx::ComputePipelineState(_computeKey);
        //    pso->setVulkanComputePipelineState(vkPipelineCache, vkPipeline);
        //    return pso;
        //}
        //
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    //bool ComputePipelineState::createVulkanComputePipelineState(const ComputePipelineStateKey & _computeKey, VkPipelineCache & _vkPipelineCache, VkPipeline & _vkPipeline)
    //{
    //    auto * device = gfx::Device::get();
    //    auto & vkDevice = device->getVulkanDevice();
    //
    //    VkPipelineCache vkPipelineCache = {};
    //
    //    VkPipelineCacheCreateInfo vkPipelineCacheDesc = {};
    //    vkPipelineCacheDesc.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    //    VG_ASSERT_VULKAN(vkCreatePipelineCache(vkDevice, &vkPipelineCacheDesc, nullptr, &vkPipelineCache));
    //
    //    VkComputePipelineCreateInfo vkPipelineDesc = {};
    //
    //    // PipelineLayout
    //    vkPipelineDesc.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    //    vkPipelineDesc.layout = device->getRootSignature(_computeKey.m_computeRootSignature)->getVulkanPipelineLayout();
    //
    //    // Shaders
    //    vector<VkPipelineShaderStageCreateInfo> vkShaderStages;
    //
    //    auto * sm = ShaderManager::get();
    //    auto & hlslDesc = *sm->getHLSLDescriptor(_computeKey.m_computeShaderKey.file);
    //
    //    if (!GraphicPipelineState::addVulkanShader(vkShaderStages, hlslDesc, ShaderStage::Compute, _computeKey.m_computeShaderKey.cs, _computeKey.m_computeShaderKey.flags))
    //        return false;
    //
    //    vkPipelineDesc.stage = vkShaderStages[0];
    //
    //    VkPipeline vkPipeline = {};
    //    VG_ASSERT_VULKAN(vkCreateComputePipelines(vkDevice, vkPipelineCache, 1, &vkPipelineDesc, nullptr, &vkPipeline));
    //
    //    _vkPipelineCache = vkPipelineCache;
    //    _vkPipeline = vkPipeline;
    //
    //    return true;
    //}

    //--------------------------------------------------------------------------------------
    RayTracingPipelineState::~RayTracingPipelineState()
    {
        //auto & vkDevice = gfx::Device::get()->getVulkanDevice();
        //
        //vkDestroyPipeline(vkDevice, m_vkComputePipeline, nullptr);
        //vkDestroyPipelineCache(vkDevice, m_vkComputePipelineCache, nullptr);
    }
}