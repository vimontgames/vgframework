#include "graphics/renderer/Precomp.h"

#include "imguiAdapter.h"

#include "imgui/imgui.cpp"
#include "imgui/imgui_demo.cpp"
#include "imgui/imgui_draw.cpp"
#include "imgui/imgui_widgets.cpp"

#include "imgui/examples/imgui_impl_win32.cpp"
#include "imgui/examples/imgui_impl_dx12.cpp"

#include "graphics/driver/Device/Device.h"
#include "graphics/driver/CommandList/CommandList.h"
#include "graphics/driver/Resource/Texture.h"
#include "graphics/driver/BindlessTable/BindlessTable.h"

namespace vg::graphics::renderer
{
    using namespace vg::graphics::driver;

    //--------------------------------------------------------------------------------------
    ImguiAdapter::ImguiAdapter(core::WinHandle _winHandle, Device & _device)
    {
        ImGui::CreateContext();
        ImGuiIO & io = ImGui::GetIO();
        // TODO: Set optional io.ConfigFlags values, e.g. 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard' to enable keyboard controls.
        // TODO: Fill optional fields of the io structure later.
        // TODO: Load TTF/OTF fonts if you don't want to use the default font.

        ImGui_ImplWin32_Init(_winHandle); 
        
        BindlessTable * bindlessTable = _device.getBindlessTable();

        m_fontTexSRVHandle = bindlessTable->allocBindlessTextureHandle((Texture*)nullptr);
        
        D3D12_CPU_DESCRIPTOR_HANDLE fontCpuHandle = bindlessTable->getd3d12CPUDescriptorHandle(m_fontTexSRVHandle);
        D3D12_GPU_DESCRIPTOR_HANDLE fontGpuHandle = bindlessTable->getd3d12GPUDescriptorHandle(m_fontTexSRVHandle);
        
        const PixelFormat fmt = _device.getBackbufferFormat();
        
        ImGui_ImplDX12_Init(_device.getd3d12Device(), max_frame_latency, Texture::getd3d12PixelFormat(fmt), bindlessTable->getd3d12GPUDescriptorHeap(), fontCpuHandle, fontGpuHandle);
    }

    //--------------------------------------------------------------------------------------
    ImguiAdapter::~ImguiAdapter()
    {
        BindlessTable * bindlessTable = Device::get()->getBindlessTable();
        bindlessTable->freeBindlessTextureHandle(m_fontTexSRVHandle);

        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    //--------------------------------------------------------------------------------------
    void ImguiAdapter::beginFrame()
    {
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    //--------------------------------------------------------------------------------------
    void ImguiAdapter::render(driver::CommandList * _cmdList)
    {
        ImGui::Render();
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), _cmdList->getd3d12GraphicsCommandList());
    }
}
