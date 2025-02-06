#ifdef VG_DX12
#include "dx12/BlendState_dx12.hpp"
#elif defined(VG_VULKAN)
#include "vulkan/BlendState_vulkan.hpp"
#else
#error Undefined GFXAPI
#endif

namespace vg::gfx
{
    VG_STATIC_ASSERT(sizeof(RenderTargetBlend) == 4, "Invalid sizeof(RenderTargetBlend)");
}