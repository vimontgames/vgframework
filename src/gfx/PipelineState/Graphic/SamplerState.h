#pragma once

#include "SamplerState_consts.h"

namespace vg::gfx
{
    vg_enum_class(Filter, core::u8,
        Nearest = 0,
        Linear,
        DepthCmp
    );

    vg_enum_class(Address, core::u8,
        Repeat = 0,
        Clamp,
        Mirror
    );

    vg_enum_class(Anisotropy, core::u8,
        Anisotropy_None,
        Anisotropy_2X,
        Anisotropy_4X,
        Anisotropy_8X,
        Anisotropy_16X
    );

    namespace base
    {
        class SamplerState
        {
        public:
            SamplerState() :
                bits(0)
            {
            }

        protected:
            union
            {
                struct
                {
                    Filter      filter     : 2;
                    Address     address    : 3;
                    Anisotropy  anisotropy : 3;
                };
                core::u8 bits;
            };
        };
    }
}

#include VG_GFXAPI_HEADER(SamplerState)

namespace vg::gfx
{
    class SamplerState : public VG_GFXAPI::SamplerState
    {
        using super = VG_GFXAPI::SamplerState;

    public:

        SamplerState(Sampler _sampler);
        SamplerState(Filter  _filter = Filter::Nearest, Address _address = Address::Repeat, Anisotropy _anisotropy = Anisotropy::Anisotropy_None);

        inline bool operator == (const SamplerState & _other) const
        {
            return _other.bits == bits;
        }

        inline bool operator != (const SamplerState & _other) const
        {
            return !(_other.bits == bits);
        }

        struct hash
        {
            inline core::size_t operator() (const SamplerState & _this) const
            {
                VG_STATIC_ASSERT(sizeof(_this) == sizeof(core::u8), "Invalid sizeof in hash function");
                return std::hash<core::u8>()(_this.bits);
            }
        };
    };
}