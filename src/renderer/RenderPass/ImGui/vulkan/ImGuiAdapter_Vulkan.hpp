#include "ImGuiAdapter_Vulkan.h"

using namespace vg::gfx;

namespace vg::renderer::vulkan
{
    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::init()
    {
        gfx::Device * device = Device::get();

        VkDescriptorPoolSize imguiDescriptorPoolSizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 16 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1 }
        };

        VkDescriptorPoolCreateInfo imguiDescriptorDesc = {};
        imguiDescriptorDesc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        imguiDescriptorDesc.pNext = nullptr;
        imguiDescriptorDesc.maxSets = max_frame_latency * s_maxImGuiTexDisplayedPerFrame;
        imguiDescriptorDesc.poolSizeCount = (uint)countof(imguiDescriptorPoolSizes);
        imguiDescriptorDesc.pPoolSizes = imguiDescriptorPoolSizes;
        imguiDescriptorDesc.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

        VG_VERIFY_VULKAN(vkCreateDescriptorPool(device->getVulkanDevice(), &imguiDescriptorDesc, nullptr, &m_vkImguiDescriptorPool));

        createRenderPass();

        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = device->getVulkanInstance();
        init_info.PhysicalDevice = device->getVulkanPhysicalDevice();
        init_info.Device = device->getVulkanDevice();
        init_info.QueueFamily = device->getVulkanCommandQueueFamilyIndex(CommandListType::Graphics);
        init_info.Queue = device->getCommandQueue(CommandQueueType::Graphics)->getVulkanCommandQueue();
        init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.DescriptorPool = m_vkImguiDescriptorPool;
        init_info.Allocator = nullptr;
        init_info.MinImageCount = max_frame_latency;
        init_info.ImageCount = max_frame_latency;
        init_info.CheckVkResultFn = nullptr;
        init_info.PipelineInfoMain.RenderPass = m_vkImguiRenderPass;

        ImGui_ImplVulkan_Init(&init_info);

        VkSamplerCreateInfo sampler_info = {};
        sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_info.magFilter = VK_FILTER_LINEAR;
        sampler_info.minFilter = VK_FILTER_LINEAR;
        sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.minLod = 0;
        sampler_info.maxLod = 0;
        sampler_info.maxAnisotropy = 0.0f;
        sampler_info.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        sampler_info.compareEnable = false;
        sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
        VG_VERIFY_VULKAN(vkCreateSampler(device->getVulkanDevice(), &sampler_info, nullptr, &m_vkSampler));
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::deinit()
    {
        ImGui_ImplVulkan_Shutdown();

        gfx::Device * device = Device::get();
        vkDestroyDescriptorPool(device->getVulkanDevice(), m_vkImguiDescriptorPool, nullptr);
        vkDestroyRenderPass(device->getVulkanDevice(), m_vkImguiRenderPass, nullptr);
        vkDestroySampler(device->getVulkanDevice(), m_vkSampler, nullptr);
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::createRenderPass()
    {
        gfx::Device * device = Device::get();

        const PixelFormat fmt = getOutputPixelFormat();
        m_vkRenderTargetFormat = Texture::getVulkanPixelFormat(fmt);

        VkAttachmentDescription attachment = {};
        attachment.format = m_vkRenderTargetFormat;
        attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference color_attachment = {};
        color_attachment.attachment = 0;
        color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_attachment;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        info.attachmentCount = 1;
        info.pAttachments = &attachment;
        info.subpassCount = 1;
        info.pSubpasses = &subpass;
        info.dependencyCount = 1;
        info.pDependencies = &dependency;

        VG_VERIFY_VULKAN(vkCreateRenderPass(device->getVulkanDevice(), &info, nullptr, &m_vkImguiRenderPass));
    }

    //--------------------------------------------------------------------------------------
    // Do not destroy all DeviceObjects using ImGui_ImplVulkan_DestroyDeviceObjects, as we 
    // just need to destroy PipelineLayout and Pipeline object and re-create RenderPass.
    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::updateBackbufferFormat()
    {
        ImGui_ImplVulkan_Data * bd = ImGui_ImplVulkan_GetBackendData();

        auto renderOutputFormat = Texture::getVulkanPixelFormat(getOutputPixelFormat());

        if (m_vkRenderTargetFormat != renderOutputFormat)
        {
            ImGui_ImplVulkan_InitInfo * v = &bd->VulkanInitInfo;

            auto device = gfx::Device::get();

            vkDestroyPipelineLayout(device->getVulkanDevice(), bd->PipelineLayout, v->Allocator);
            bd->PipelineLayout = nullptr;

            vkDestroyPipeline(device->getVulkanDevice(), bd->Pipeline, v->Allocator);
            bd->Pipeline = nullptr;

            vkDestroyRenderPass(device->getVulkanDevice(), v->PipelineInfoMain.RenderPass, nullptr);
            v->PipelineInfoMain.RenderPass = nullptr;

            createRenderPass();

            v->PipelineInfoMain.RenderPass = m_vkImguiRenderPass;
            v->PipelineInfoMain.Subpass = 0;
            v->PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

            ImGui_ImplVulkan_CreateDeviceObjects();
            m_vkRenderTargetFormat = renderOutputFormat;
        }
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::beginFrame()
    {
        ImGui_ImplVulkan_NewFrame();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::render(gfx::CommandList * _cmdList)
    {
        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), _cmdList->getVulkanCommandBuffer(), nullptr);
    }

    //--------------------------------------------------------------------------------------
    // In case of crash increase size of VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER in init()
    //--------------------------------------------------------------------------------------
    ImTextureID ImGuiAdapter::addTexture(const gfx::Texture * _texture)
    {
        return (ImTextureID)ImGui_ImplVulkan_AddTexture(m_vkSampler, _texture->getVulkanImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::removeTexture(ImTextureID id)
    {
        ImGui_ImplVulkan_RemoveTexture((VkDescriptorSet)id);
    }
}