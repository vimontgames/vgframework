#pragma once

#include "SamplerState_consts.h"

namespace vg::gfx
{
    enum class Filter : core::u8
    {
        Nearest = 0,
        Linear
    };

    enum class Address : core::u8
    {
        Repeat = 0,
        Clamp,
        Mirror,
    };

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
                    Filter      filter  : 1;
                    Address     address : 2;

                };
                core::u32 bits;
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
        SamplerState(Filter  _filter = Filter::Nearest, Address _address = Address::Repeat);

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
                VG_STATIC_ASSERT(sizeof(_this) == sizeof(core::u32), "Invalid sizeof in hash function");
                return std::hash<core::u32>()(_this.bits);
            }
        };
    };
}