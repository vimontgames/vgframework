#include VG_GFXAPI_IMPL(DepthStencilState)

namespace vg::gfx::base
{
    VG_STATIC_ASSERT(sizeof(StencilState) == sizeof(u16), "Invalid sizeof(StencilState)");
    VG_STATIC_ASSERT(sizeof(DepthStencilState) == sizeof(u64), "Invalid sizeof(DepthStencilState)");

}