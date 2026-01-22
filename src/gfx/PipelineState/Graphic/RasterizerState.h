#pragma once

#include "RasterizerState_consts.h"
#include "Shaders/system/device_consts.hlsli"

namespace vg::gfx
{
    namespace base
    {
        class RasterizerState
        {
        public:
            RasterizerState() : 
                bits(0)
            {
            }

            VG_INLINE FillMode    getFillMode       () const { return fillMode; }
            VG_INLINE CullMode    getCullMode       () const { return cullMode; }
            VG_INLINE Orientation getOrientation    () const { return orientation; }
            VG_INLINE DepthClip   getDepthClip      () const { return depthClip; }
            VG_INLINE DepthBias   getDepthBias      () const { return depthBias; }

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

#ifdef VG_DX12
#include "dx12/RasterizerState_dx12.h"
#elif defined(VG_VULKAN)
#include "vulkan/RasterizerState_vulkan.h"
#else
#error Undefined GFXAPI
#endif

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