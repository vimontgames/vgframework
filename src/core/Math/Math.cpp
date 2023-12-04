#include "core/Precomp.h"
#include "Math.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    float4x4 extractRotation(const float4x4 & _matrix)
    {
        float4x4 rot;
        rot[0] = float4(normalize(_matrix[0].xyz), 0.0f);
        rot[1] = float4(normalize(_matrix[1].xyz), 0.0f);
        rot[2] = float4(normalize(_matrix[2].xyz), 0.0f);
        rot[3] = float4(0.0f, 0.0f, 0.0f, 1.0f);
        return rot;
    }

    //--------------------------------------------------------------------------------------
    quaternion getQuaternionFromRotationMatrix(const float4x4 & _matrix)
    {
        const float m00 = _matrix[0].x;
        const float m01 = _matrix[0].y;
        const float m02 = _matrix[0].z;

        const float m10 = _matrix[1].x;
        const float m11 = _matrix[1].y;
        const float m12 = _matrix[1].z;

        const float m20 = _matrix[2].x;
        const float m21 = _matrix[2].y;
        const float m22 = _matrix[2].z;

        const float tr = m00 + m11 + m22;

        float qx, qy, qz, qw;

        if (tr > 0.0f)
        {
            const float S = sqrtf(tr + 1.0f) * 2.0f;
            
            qx = (m21 - m12) / S;
            qy = (m02 - m20) / S;
            qz = (m10 - m01) / S;
            qw = 0.25f * S;
        }
        else if ((m00 > m11) & (m00 > m22))
        {
            const float S = sqrtf(1.0f + m00 - m11 - m22) * 2.0f;
            
            qx = 0.25f * S;
            qy = (m01 + m10) / S;
            qz = (m02 + m20) / S;
            qw = (m21 - m12) / S;
        }
        else if (m11 > m22)
        {
            const float S = sqrtf(1.0f + m11 - m00 - m22) * 2.0f;
            
            qx = (m01 + m10) / S;
            qy = 0.25f * S;
            qz = (m12 + m21) / S;
            qw = (m02 - m20) / S;
        }
        else
        {
            const float S = sqrtf(1.0f + m22 - m00 - m11) * 2.0f;
            
            qx = (m02 + m20) / S;
            qy = (m12 + m21) / S;
            qz = 0.25f * S;
            qw = (m10 - m01) / S;
        }

        return quaternion(qx, qy, qz, qw);
    }
}