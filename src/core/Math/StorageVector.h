#pragma once

namespace vg::core
{
    struct StorageFloat2
    {
        float x, y;

        inline StorageFloat2()
        {

        }

        inline StorageFloat2(core::float2 _value) :
            x(_value.x),
            y(_value.y)
        {

        }

        inline explicit operator core::float2() const
        {
            return core::float2(x, y);
        }

        inline bool operator == (const StorageFloat2 & _other) const
        {
            return x == _other.x && y == _other.y;
        }
    };

    struct StorageFloat3
    {
        float x, y, z;

        inline StorageFloat3()
        {

        }

        inline StorageFloat3(core::float3 _value) :
            x(_value.x),
            y(_value.y),
            z(_value.z)
        {

        }

        inline explicit operator core::float3() const
        {
            return core::float3(x, y, z);
        }

        inline bool operator == (const StorageFloat3 & _other) const
        {
            return x == _other.x && y == _other.y && z == _other.z;
        }
    };

    struct StorageFloat4
    {
        float x, y, z, w;

        inline StorageFloat4()
        {

        }

        inline StorageFloat4(core::float4 _value) :
            x(_value.x),
            y(_value.y),
            z(_value.z),
            w(_value.w)
        {

        }

        inline explicit operator core::float4() const
        {
            return core::float4(x, y, z, w);
        }

        inline bool operator == (const StorageFloat4 & _other) const
        {
            return x == _other.x && y == _other.y && z == _other.z && w == _other.w;
        }
    };
}