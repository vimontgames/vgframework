#ifdef VG_DX12
#include "dx12/DepthStencilState_dx12.hpp"
#elif defined(VG_VULKAN)
#include "vulkan/DepthStencilState_vulkan.hpp"
#else
#error Undefined GFXAPI
#endif

namespace vg::gfx::base
{
    VG_STATIC_ASSERT(sizeof(StencilState) == sizeof(u16), "Invalid sizeof(StencilState)");
    VG_STATIC_ASSERT(sizeof(DepthStencilState) == sizeof(u64), "Invalid sizeof(DepthStencilState)");

}