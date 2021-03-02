#include VG_GFXAPI_IMPL(DepthStencilState)

namespace vg::graphics::driver::base
{
    VG_STATIC_ASSERT(sizeof(StencilState) == 2, "Invalid sizeof(StencilState)");
    VG_STATIC_ASSERT(sizeof(DepthStencilState) == 8, "Invalid sizeof(DepthStencilState)");

}