#include "ImGuiAdapter_dx12.h"

using namespace vg::gfx;

namespace vg::renderer::dx12
{
    struct ImGuiDX12DescriptorInfo
    {
        gfx::BindlessTextureHandle texHandle;
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
        bool dirty;
    };
    static core::vector<ImGuiDX12DescriptorInfo> g_ImGuiDescriptors;

    //--------------------------------------------------------------------------------------
    void allocDescriptorCallback(ImGui_ImplDX12_InitInfo * info, D3D12_CPU_DESCRIPTOR_HANDLE * out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE * out_gpu_desc_handle)
    {
        gfx::Device * device = Device::get();
        BindlessTable * bindlessTable = device->getBindlessTable();

        ImGuiDX12DescriptorInfo descInfo;
        descInfo.texHandle = bindlessTable->allocBindlessTextureHandle(nullptr);
        descInfo.cpuHandle = bindlessTable->getd3d12CPUDescriptorHandle(descInfo.texHandle);
        descInfo.gpuHandle = bindlessTable->getd3d12GPUDescriptorHandle(descInfo.texHandle);
        descInfo.dirty = true;

        g_ImGuiDescriptors.push_back(descInfo);

        *out_cpu_desc_handle = descInfo.cpuHandle;
        *out_gpu_desc_handle = descInfo.gpuHandle;
    }

    //--------------------------------------------------------------------------------------
    void freeDescriptorCallback(ImGui_ImplDX12_InitInfo * info, D3D12_CPU_DESCRIPTOR_HANDLE cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_desc_handle)
    {
        gfx::Device * device = Device::get();
        BindlessTable * bindlessTable = device->getBindlessTable();

        uint index = -1;
        for (uint i = 0; i < g_ImGuiDescriptors.size(); ++i)
        {
            ImGuiDX12DescriptorInfo & descInfo = g_ImGuiDescriptors[i];
            if (cpu_desc_handle.ptr == descInfo.cpuHandle.ptr)
            {
                index = i;
                break;
            }
        }
        if (-1 != index)
        {
            ImGuiDX12DescriptorInfo & descInfo = g_ImGuiDescriptors[index];
            bindlessTable->freeBindlessTextureHandle(descInfo.texHandle);
            g_ImGuiDescriptors.erase(g_ImGuiDescriptors.begin() + index);
        }
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::init()
    {
        gfx::Device * device = Device::get();
        BindlessTable * bindlessTable = device->getBindlessTable();
        const PixelFormat fmt = getOutputPixelFormat();


        ImGui_ImplDX12_InitInfo init_info = {};
        init_info.Device = device->getd3d12Device();
        init_info.CommandQueue = device->getCommandQueue(CommandQueueType::Graphics)->getd3d12CommandQueue();
        init_info.NumFramesInFlight = max_frame_latency;
        init_info.RTVFormat = Texture::getd3d12SurfaceFormat(fmt);
        init_info.SrvDescriptorHeap = bindlessTable->getd3d12GPUDescriptorHeap();
        init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo * info, D3D12_CPU_DESCRIPTOR_HANDLE * out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE * out_gpu_handle) { return allocDescriptorCallback(info, out_cpu_handle, out_gpu_handle); };
        init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo * info, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle) { return freeDescriptorCallback(info, cpu_handle, gpu_handle); };

        ImGui_ImplDX12_Init(&init_info);
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::deinit()
    {
        ImGui_ImplDX12_Shutdown();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::updateBackbufferFormat()
    {
        auto device = gfx::Device::get();

        // update backbuffer format
        ImGui_ImplDX12_Data * bd = ImGui_ImplDX12_GetBackendData();

        auto fmt = getOutputPixelFormat();
        auto renderOutputFormat = Texture::getd3d12ResourceFormat(fmt);

        if (bd->RTVFormat != renderOutputFormat)
        {
            auto * renderer = Renderer::get();
            renderer->WaitGPUIdle();

            //ImGui_ImplDX12_Shutdown();

            ImGui_ImplDX12_Data * bd = ImGui_ImplDX12_GetBackendData();
            if (bd && bd->pd3dDevice)
            {
                // Do not destroy all DeviceObjects as we don't want to recreate everything and doing so is also unstable
                //ImGui_ImplDX12_InvalidateDeviceObjects();

                SafeRelease(bd->pRootSignature);
                SafeRelease(bd->pPipelineState);
                SafeRelease(bd->pdxgiFactory);
                SafeRelease(bd->pTexCmdAllocator);
                SafeRelease(bd->pTexCmdList);
                SafeRelease(bd->Fence);
                bd->RTVFormat = renderOutputFormat;
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::updateDescriptors()
    {
        gfx::Device * device = Device::get();
        BindlessTable * bindlessTable = device->getBindlessTable();

        for (uint i = 0; i < g_ImGuiDescriptors.size(); ++i)
        {
            if (g_ImGuiDescriptors[i].dirty)
            {
                bindlessTable->updated3d12descriptor(g_ImGuiDescriptors[i].texHandle);
                g_ImGuiDescriptors[i].dirty = false;
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::beginFrame()
    {
        ImGui_ImplDX12_NewFrame();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::render(gfx::CommandList * _cmdList)
    {
        ImGui::Render();
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), _cmdList->getd3d12GraphicsCommandList());
        updateDescriptors();
    }

    //--------------------------------------------------------------------------------------
    ImTextureID ImGuiAdapter::addTexture(const gfx::Texture * _texture)
    {
        auto device = gfx::Device::get();
        gfx::BindlessTable * bindlessTable = device->getBindlessTable();
        return  (ImTextureID)bindlessTable->getd3d12GPUDescriptorHandle(_texture->getTextureHandle()).ptr;
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::removeTexture(ImTextureID id)
    {
        // Nothing to do here, unlike Vulkan version we just returned an existing CPU handle as ID
    }
}