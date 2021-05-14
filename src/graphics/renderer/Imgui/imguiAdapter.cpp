#include "graphics/renderer/Precomp.h"

#include "imguiAdapter.h"

#include "imgui/imgui.cpp"
#include "imgui/imgui_demo.cpp"
#include "imgui/imgui_draw.cpp"
#include "imgui/imgui_tables.cpp"
#include "imgui/imgui_widgets.cpp"

#ifdef _WIN32
#include "imgui/backends/imgui_impl_win32.cpp"
#endif

#ifdef VG_DX12
#include "imgui/backends/imgui_impl_dx12.cpp"
#elif defined(VG_VULKAN)
#include "imgui/backends/imgui_impl_vulkan.cpp"
#endif

#include "graphics/driver/Device/Device.h"
#include "graphics/driver/CommandList/CommandList.h"
#include "graphics/driver/CommandQueue/CommandQueue.h"
#include "graphics/driver/Resource/Texture.h"
#include "graphics/driver/BindlessTable/BindlessTable.h"

using namespace vg::core;
using namespace vg::graphics::driver;

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    ImguiAdapter::ImguiAdapter(WinHandle _winHandle, Device & _device)
    {
        ImGui::CreateContext();
        ImGuiIO & io = ImGui::GetIO();
        // TODO: Set optional io.ConfigFlags values, e.g. 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard' to enable keyboard controls.
        // TODO: Fill optional fields of the io structure later.
        // TODO: Load TTF/OTF fonts if you don't want to use the default font.

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable;
        io.ConfigDockingTransparentPayload = true;

        ImGui::StyleColorsDark();
        //auto & style = ImGui::GetStyle();
        //style.Colors[ImGuiCol_TitleBg] = ImVec4(1.0f,0.0f,0.0f, 1.0f);

        #ifdef _WIN32
        ImGui_ImplWin32_Init(_winHandle); 
        #endif

        BindlessTable * bindlessTable = _device.getBindlessTable();
        m_fontTexSRVHandle = bindlessTable->allocBindlessTextureHandle((Texture*)nullptr, ReservedSlot(bindless_texture_SRV_invalid-1));

        #ifdef VG_DX12
        d3d12Init();
        #elif defined(VG_VULKAN)
        vulkanInit();
        #endif 
    }

    #ifdef VG_DX12
    //--------------------------------------------------------------------------------------
    void ImguiAdapter::d3d12Init()
    {
        driver::Device * device = Device::get();

        const PixelFormat fmt = device->getBackbufferFormat();
        BindlessTable * bindlessTable = device->getBindlessTable();

        D3D12_CPU_DESCRIPTOR_HANDLE fontCpuHandle = bindlessTable->getd3d12CPUDescriptorHandle(m_fontTexSRVHandle);
        D3D12_GPU_DESCRIPTOR_HANDLE fontGpuHandle = bindlessTable->getd3d12GPUDescriptorHandle(m_fontTexSRVHandle);

        ImGui_ImplDX12_Init(device->getd3d12Device(), max_frame_latency, Texture::getd3d12PixelFormat(fmt), bindlessTable->getd3d12GPUDescriptorHeap(), fontCpuHandle, fontGpuHandle);
    }
    #elif defined(VG_VULKAN)
    //--------------------------------------------------------------------------------------
    void ImguiAdapter::vulkanInit()
    {
        driver::Device * device = Device::get();

        VkDescriptorPoolSize imguiDescriptorPoolSizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };

        VkDescriptorPoolCreateInfo imguiDescriptorDesc = {};
        imguiDescriptorDesc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        imguiDescriptorDesc.pNext = nullptr;
        imguiDescriptorDesc.maxSets = max_frame_latency;
        imguiDescriptorDesc.poolSizeCount = (uint)countof(imguiDescriptorPoolSizes);
        imguiDescriptorDesc.pPoolSizes = imguiDescriptorPoolSizes;

        VG_ASSERT_VULKAN(vkCreateDescriptorPool(device->getVulkanDevice(), &imguiDescriptorDesc, nullptr, &m_vkImguiDescriptorPool));

        const PixelFormat fmt = device->getBackbufferFormat();

        VkAttachmentDescription attachment = {};
        attachment.format = Texture::getVulkanPixelFormat(fmt);
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

        VG_ASSERT_VULKAN(vkCreateRenderPass(device->getVulkanDevice(), &info, nullptr, &m_vkImguiRenderPass));

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

        ImGui_ImplVulkan_Init(&init_info, m_vkImguiRenderPass);
    }
    #endif

    //--------------------------------------------------------------------------------------
    ImguiAdapter::~ImguiAdapter()
    {
        driver::Device * device = Device::get();

        device->waitGPUIdle();

        BindlessTable * bindlessTable = device->getBindlessTable();
        bindlessTable->freeBindlessTextureHandle(m_fontTexSRVHandle);

        #ifdef VG_DX12
        ImGui_ImplDX12_Shutdown();
        #elif defined(VG_VULKAN)
        vkDestroyDescriptorPool(device->getVulkanDevice(), m_vkImguiDescriptorPool, nullptr);
        vkDestroyRenderPass(device->getVulkanDevice(), m_vkImguiRenderPass, nullptr);
        ImGui_ImplVulkan_Shutdown();
        #endif

        #ifdef _WIN32
        ImGui_ImplWin32_Shutdown();
        #endif //_WIN32

        ImGui::DestroyContext();
    }

    //--------------------------------------------------------------------------------------
    bool ImguiAdapter::isFocused() const
    {
        const ImGuiIO & io = ImGui::GetIO();
        if (io.WantCaptureKeyboard)
            return true;
        else
            return false;
    }

    //--------------------------------------------------------------------------------------
    void ImguiAdapter::beginFrame()
    {
        VG_PROFILE_CPU("Dear Imgui");

        driver::Device * device = Device::get();
        static bool firstFrame = true;

        #ifdef VG_DX12

        ImGui_ImplDX12_NewFrame();

        if (firstFrame)
        {
            BindlessTable * bindlessTable = device->getBindlessTable();
            bindlessTable->updated3d12descriptor(m_fontTexSRVHandle);

            firstFrame = false;
        }

        #elif defined(VG_VULKAN)

        ImGui_ImplVulkan_NewFrame();
        
        if (firstFrame)
        {
           
            CommandList * cmdList = device->getCommandLists(CommandListType::Graphics)[0];
            ImGui_ImplVulkan_CreateFontsTexture(cmdList->getVulkanCommandBuffer());

            firstFrame = false;
        }
        #endif

        #ifdef _WIN32
        ImGui_ImplWin32_NewFrame();
        #endif

        ImGui::NewFrame();
    }

    //--------------------------------------------------------------------------------------
    void ImguiAdapter::render(driver::CommandList * _cmdList)
    {
        ImGui::Render();

        #ifdef VG_DX12
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), _cmdList->getd3d12GraphicsCommandList());
        #elif defined(VG_VULKAN)
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), _cmdList->getVulkanCommandBuffer(), nullptr);
        #endif
    }
}
