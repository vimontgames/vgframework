#include "core/Precomp.h"
#include "Math.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    float3x3 extractRotation(const float4x4 & _matrix)
    {
        float3x3 rot;
        rot[0] = float3(normalize(_matrix[0].xyz));
        rot[1] = float3(normalize(_matrix[1].xyz));
        rot[2] = float3(normalize(_matrix[2].xyz));
        return rot;
    }
}