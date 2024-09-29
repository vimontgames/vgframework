namespace vg::core
{
    //--------------------------------------------------------------------------------------
    // Extract translation, rotation (in degrees) and scale from a float4x4 matrix
    //--------------------------------------------------------------------------------------
    VG_INLINE void Float4x4ToTRS(const float4x4 & _mat44, float3 & _translation, float3 & _rotation, float3 & _scale)
    {
        _scale.x = length(_mat44[0].xyz);
        _scale.y = length(_mat44[1].xyz);
        _scale.z = length(_mat44[2].xyz);

        float3 I = _mat44[0].xyz / _scale.x;
        float3 J = _mat44[1].xyz / _scale.y;
        float3 K = _mat44[2].xyz / _scale.z;

        _rotation = (float3)(180.0f / PI) * atan2(float3(J.z, -I.z, I.y), float3(K.z, sqrtf(J.z * J.z + K.z * K.z), I.x));

        _translation = _mat44[3].xyz;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE float4x4 TRSToFloat4x4(const float3 & _translation, const float3 & _rotation, const float3 & _scale)
    {
        const float3 rotationInRadians = (PI / 180.0f) * _rotation;

        float4x4 rot[3];
        rot[0] = float4x4::rotation_x(rotationInRadians.x);
        rot[1] = float4x4::rotation_y(rotationInRadians.y);
        rot[2] = float4x4::rotation_z(rotationInRadians.z);

        float4x4 mat = mul(rot[2], mul(rot[1], rot[0]));

        const float3 scale = select(abs(_scale) < FLT_EPSILON, (float3)0.001f * sign(_scale), _scale);

        mat[0] = mat[0] * scale.x;
        mat[1] = mat[1] * scale.y;
        mat[2] = mat[2] * scale.z;
        mat[3] = float4(_translation.xyz, 1.0f);

        return mat;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE float4x4 TRSToFloat4x4(const float3 & _translation, const quaternion _rotation, const float3 _scale)
    {
        quaternion q = _rotation;

        float xx = q.x * q.x, xy = q.x * q.y, xz = q.x * q.z, xw = q.x * q.w;
        float yy = q.y * q.y, yz = q.y * q.z, yw = q.y * q.w;
        float zz = q.z * q.z, zw = q.z * q.w;
        float sx = 2.0f * _scale.x, sy = 2.0f * _scale.y, sz = 2.0f * _scale.z;

        return float4x4(
            sx * (-yy - zz + 0.5f), sy * (-zw + xy), sz * (+xz + yw), _translation.x,
            sx * (+xy + zw), sy * (-xx - zz + 0.5f), sz * (-xw + yz), _translation.y,
            sx * (-yw + xz), sy * (+xw + yz), sz * (-xx - yy + 0.5f), _translation.z,
            0, 0, 0, 1
        );
    }
}