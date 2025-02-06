#ifdef VG_DX12
#include "dx12/RasterizerState_dx12.hpp"
#elif defined(VG_VULKAN)
#include "vulkan/RasterizerState_vulkan.hpp"
#else
#error Undefined GFXAPI
#endif

namespace vg::gfx
{
    VG_STATIC_ASSERT(sizeof(base::RasterizerState) == sizeof(u32), "Invalid sizeof(RasterizerState)");
}