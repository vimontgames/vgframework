#pragma once

#include "core/Precomp.h"

//--------------------------------------------------------------------------------------
// core includes
//--------------------------------------------------------------------------------------
#include "core/Types/Types.h"
#include "core/Singleton/Singleton.h"
#include "core/IProfiler.h"
#include "core/Object/AutoRegisterClass.h" 

//--------------------------------------------------------------------------------------
// Debug macros
//--------------------------------------------------------------------------------------
#ifdef VG_DEBUG
    #define VG_ENABLE_GPU_MARKER 1
    #define VG_ENABLE_SHADER_COMPILATION 1
#elif defined(VG_DEVELOPMENT)
    #define VG_ENABLE_GPU_MARKER 1
    #define VG_ENABLE_SHADER_COMPILATION 1
#elif defined(VG_RELEASE)
    #define VG_ENABLE_GPU_MARKER 1
    #define VG_ENABLE_SHADER_COMPILATION 1
#elif defined(VG_FINAL)
    #define VG_ENABLE_GPU_MARKER 0
    #define VG_ENABLE_SHADER_COMPILATION 1  // TODO: Runtime shader compilation should be disabled in VG_FINAL 
#endif

#ifdef _WIN32
#define VG_VERIFY_SUCCEEDED(f) { HRESULT hr = f; VG_ASSERT(SUCCEEDED(hr), "%s\n\n%s", #f, std::system_category().message(hr).c_str()); }
#endif

#ifdef VG_WINDOWS
#define VG_DXGI 1 // Use Windows DXGI interface to get HDR monitor status or video memory available 
#endif

#ifdef VG_DX12
//--------------------------------------------------------------------------------------
// d3d12
//--------------------------------------------------------------------------------------
#if VG_ENABLE_GPU_MARKER 
#define USE_PIX
#define PROFILE_BUILD 
#endif

#include <d3d12.h>
#include <DXGI1_6.h>
#include <dxgidebug.h>

#if VG_ENABLE_GPU_MARKER 
#include "winpixeventruntime\Include\WinPixEventRuntime\pix3.h"
#endif

#pragma push_macro("new")
#undef new
#include "D3D12MemoryAllocator/src/D3D12MemAlloc.h"
#pragma pop_macro("new")

#endif // VG_DX12

#ifdef VG_VULKAN
//--------------------------------------------------------------------------------------
// vulkan
//--------------------------------------------------------------------------------------
#include <vulkan/vulkan.h>

#ifdef VG_WINDOWS
#include <vulkan/vulkan_win32.h>
#endif

#pragma push_macro("new")
#undef new
#include "VulkanMemoryAllocator/include/vk_mem_alloc.h"
#pragma pop_macro("new")

#define VG_VERIFY_VULKAN(f) { VkResult err = f; VG_ASSERT(err == VK_SUCCESS, "%s\n\n%s", #f, Device::getVulkanErrorString(err)); }

#endif // VG_VULKAN

//--------------------------------------------------------------------------------------
// Use for static inheritance of platform-specific implementation classes
//--------------------------------------------------------------------------------------

#ifdef VG_DX12
#define VG_GFXAPI dx12
#elif defined(VG_VULKAN)
#define VG_GFXAPI vulkan
#else
#error Undefined GFXAPI
#endif

//--------------------------------------------------------------------------------------
// Raytracing
//--------------------------------------------------------------------------------------

#ifdef VG_DX12
#define VG_ENABLE_RAYTRACING 1
#elif defined(VG_VULKAN)
#define VG_ENABLE_RAYTRACING 1
#endif