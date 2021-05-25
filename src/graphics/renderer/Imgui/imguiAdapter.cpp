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
    void FiftyShadesOfGreyStyle()
    {
        ImGuiStyle & style = ImGui::GetStyle();
        ImVec4 * colors = style.Colors;

        float rounding = 3.0f; 

        style.PopupRounding = rounding;
        style.WindowPadding = ImVec2(4, 4);
        style.FramePadding = ImVec2(4, 4);
        style.ItemSpacing = ImVec2(4, 4);
        style.ItemInnerSpacing = ImVec2(4, 4);
        style.ScrollbarSize = 16;
        style.IndentSpacing = 16;
        style.WindowBorderSize = 0;
        style.ChildBorderSize = 0;
        style.PopupBorderSize = 0;
        style.FrameBorderSize = 0;
        style.WindowRounding = rounding;
        style.ChildRounding = rounding;
        style.FrameRounding = rounding;
        style.ScrollbarRounding = rounding;
        style.GrabRounding = rounding;
        style.Alpha = 0.9f;

        #ifdef IMGUI_HAS_DOCK 
        style.TabBorderSize = 0;
        style.TabRounding = rounding;
        #endif

        colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 0.75f);
        colors[ImGuiCol_TextDisabled]           = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
        colors[ImGuiCol_WindowBg]               = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
        colors[ImGuiCol_ChildBg]                = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
        colors[ImGuiCol_PopupBg]                = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
        colors[ImGuiCol_Border]                 = ImVec4(0.13f, 0.13f, 0.14f, 1.00f);
        colors[ImGuiCol_BorderShadow]           = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_FrameBg]                = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_TitleBg]                = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.23f, 0.34f, 0.54f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.13f, 0.13f, 0.15f, 0.50f);
        colors[ImGuiCol_MenuBarBg]              = ImVec4(0.13f, 0.13f, 0.15f, 1.00f);
        colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_CheckMark]              = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
        colors[ImGuiCol_SliderGrab]             = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.33f, 0.42f, 0.60f, 1.00f);
        colors[ImGuiCol_Button]                 = ImVec4(0.23f, 0.34f, 0.54f, 0.77f);
        colors[ImGuiCol_ButtonHovered]          = ImVec4(0.23f, 0.34f, 0.54f, 1.00f);
        colors[ImGuiCol_ButtonActive]           = ImVec4(0.33f, 0.42f, 0.60f, 1.00f);
        colors[ImGuiCol_Header]                 = ImVec4(0.23f, 0.34f, 0.54f, 0.77f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.23f, 0.34f, 0.54f, 1.00f);
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.33f, 0.42f, 0.60f, 1.00f);
        colors[ImGuiCol_Separator]              = ImVec4(0.23f, 0.34f, 0.54f, 0.77f);
        colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.23f, 0.34f, 0.54f, 1.00f);
        colors[ImGuiCol_SeparatorActive]        = ImVec4(0.33f, 0.42f, 0.60f, 1.00f);
        colors[ImGuiCol_ResizeGrip]             = ImVec4(0.23f, 0.34f, 0.54f, 0.50f);
        colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.23f, 0.34f, 0.54f, 0.77f);
        colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.33f, 0.42f, 0.60f, 0.77f);
        colors[ImGuiCol_Tab]                    = ImVec4(0.33f, 0.42f, 0.60f, 0.50f);
        colors[ImGuiCol_TabHovered]             = ImVec4(0.33f, 0.42f, 0.60f, 1.00f);
        colors[ImGuiCol_TabActive]              = ImVec4(0.33f, 0.42f, 0.60f, 1.00f);
        colors[ImGuiCol_TabUnfocused]           = ImVec4(0.54f, 0.54f, 0.54f, 0.19f);
        colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.54f, 0.54f, 0.54f, 0.38f);
        colors[ImGuiCol_DockingPreview]         = ImVec4(0.20f, 0.29f, 0.43f, 0.77f);
        colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.13f, 0.23f, 0.36f, 0.50f);
        colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
        colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
        colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
        colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
        colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    }

    //--------------------------------------------------------------------------------------
    ImguiAdapter::ImguiAdapter(WinHandle _winHandle, Device & _device)
    {
        ImGui::CreateContext();
        ImGuiIO & io = ImGui::GetIO();

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable;
        io.ConfigDockingTransparentPayload = true;

        FiftyShadesOfGreyStyle();

        io.Fonts->AddFontFromFileTTF("data/Fonts/ubuntu/UbuntuMono-R.ttf", 15);
        //io.Fonts->AddFontFromFileTTF("data/Fonts/roboto/static/RobotoMono-Regular.ttf", 16);

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
