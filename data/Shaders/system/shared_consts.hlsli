#pragma once

//--------------------------------------------------------------------------------------
// Reverse Z-Buffer direction to improve precision (1 = near plane and 0 = far plane when enabled)
// Reverse Z-Buffer  is urrently disabled, because we need to fix:
// - DOF
// - AlphaBlend depth transparency
// - Anything else?
//--------------------------------------------------------------------------------------

#define VG_GFX_REVERSE_DEPTH 0