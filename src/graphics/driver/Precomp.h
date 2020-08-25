#include "core/Precomp.h"

//--------------------------------------------------------------------------------------
// core includes
//--------------------------------------------------------------------------------------
#include "core/Types/Types.h"
#include "core/Singleton/Singleton.h"

//--------------------------------------------------------------------------------------
// Debug macros
//--------------------------------------------------------------------------------------
#ifdef VG_DEBUG
#define VG_ENABLE_GPU_MARKER 1
#elif defined(VG_RELEASE)
#define VG_ENABLE_GPU_MARKER 1
#endif

#ifdef VG_DX12
//--------------------------------------------------------------------------------------
// d3d12
//--------------------------------------------------------------------------------------
#include <d3d12.h>
#include <DXGI1_5.h>
#include <dxgidebug.h>
#if VG_ENABLE_GPU_MARKER 
#include <pix_win.h>
#endif

#define VG_ASSERT_SUCCEEDED(f) { HRESULT hr = f; VG_ASSERT(SUCCEEDED(hr), "%s\n\n%s", #f, std::system_category().message(hr).c_str()); }

#endif // VG_DX12

#ifdef VG_VULKAN
//--------------------------------------------------------------------------------------
// vulkan
//--------------------------------------------------------------------------------------
#include <vulkan/vulkan.h>

#ifdef VG_WINDOWS
#include <vulkan/vulkan_win32.h>
#endif

#define VG_ASSERT_VULKAN(f) { VkResult err = f; VG_ASSERT(err == VK_SUCCESS, "%s\n\n%s", #f, (core::asString(err)).c_str()); }

#endif // VG_VULKAN

//--------------------------------------------------------------------------------------
// Use for static inheritance of platform-specific implementation classes
//--------------------------------------------------------------------------------------

#define VG_STRINGIFY(quoteMe) #quoteMe

#define VG_GRAPHICSAPI_MAKE_HEADER_PATH(filename, api)			VG_STRINGIFY(api##/filename##_##api.h)
#define VG_GRAPHICSAPI_MAKE_IMPLEMENTATION_PATH(filename, api)	VG_STRINGIFY(api##/filename##_##api.hpp)

#ifdef VG_DX12

#define VG_GRAPHICSAPI dx12
#define VG_GRAPHICSAPI_HEADER(filename) VG_GRAPHICSAPI_MAKE_HEADER_PATH(filename, dx12)
#define VG_GRAPHICSAPI_IMPLEMENTATION(filename) VG_GRAPHICSAPI_MAKE_IMPLEMENTATION_PATH(filename, dx12)

#elif defined(VG_VULKAN)

#define VG_GRAPHICSAPI vulkan
#define VG_GRAPHICSAPI_HEADER(filename) VG_GRAPHICSAPI_MAKE_HEADER_PATH(filename, vulkan)
#define VG_GRAPHICSAPI_IMPLEMENTATION(filename) VG_GRAPHICSAPI_MAKE_IMPLEMENTATION_PATH(filename, vulkan)

#endif