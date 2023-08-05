#pragma once

namespace vg::gfx
{
    enum class FillMode : core::u32
    {
        Solid = 0,
        Wireframe
    };

    enum class CullMode : core::u32
    {
        None = 0,
        Back,
        Front
    };

    enum class Orientation : core::u32
    {
        ClockWise = 0,
        CounterClockWise
    };

    enum class DepthClip : core::u32
    {
        Enable = 1,
        Disable
    };

    enum class DepthBias : core::u32
    {
        None,
        Small,
        Medium,
        Big
    };

    namespace base
    {
        class RasterizerState
        {
        public:
            RasterizerState() : 
                bits(0)
            {
            }

        protected:
            union
            {
                struct
                {
                    FillMode    fillMode    : 1;
                    CullMode    cullMode    : 2;
                    Orientation orientation : 1;
                    DepthClip   depthClip   : 1;
                    DepthBias   depthBias   : 2; 
                };
                core::u32 bits;
            };
        };
    }
}

#include VG_GFXAPI_HEADER(RasterizerState)

namespace vg::gfx
{
    class RasterizerState : public VG_GFXAPI::RasterizerState
    {
        using super = VG_GFXAPI::RasterizerState;

    public:

        RasterizerState(FillMode _fillMode       = FillMode::Solid, 
                        CullMode _cullMode       = CullMode::Back, 
                        Orientation _orientation = Orientation::ClockWise, 
                        DepthClip _depthClip     = DepthClip::Enable,
                        DepthBias _depthBias     = DepthBias::None)
            {
                fillMode    = _fillMode;
                cullMode    = _cullMode;
                orientation = _orientation;
                depthClip   = _depthClip;
                depthBias   = _depthBias;
            }

        inline bool operator == (const RasterizerState & _other) const
        {
            return _other.bits == bits;
        }

        inline bool operator != (const RasterizerState & _other) const
        {
            return !(_other.bits == bits);
        }

        struct hash
        {
            inline core::size_t operator() (const RasterizerState & _this) const
            {
                VG_STATIC_ASSERT(sizeof(_this) == sizeof(core::u32), "Invalid sizeof in hash function");
                return std::hash<core::u32>()(_this.bits);
            }
        };
    };
}