#pragma once

namespace vg::graphics::driver
{
    namespace base
    {
        class RasterizerState
        {
        public:
            RasterizerState() :
                bits(0)
            {
                m_fillMode = FillMode::Solid;
                m_cullMode = CullMode::Back;
            }

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

            void setFillmode(FillMode _fillMode) { m_fillMode = _fillMode; }
            FillMode getFillmode() const { return m_fillMode; }

            void setCullMode(CullMode _cullMode) { m_cullMode = _cullMode; }
            CullMode getCullmode() const { return m_cullMode; }

        protected:
            union
            {
                struct
                {
                    FillMode    m_fillMode : 1;
                    CullMode    m_cullMode : 2;
                };
                core::u32 bits;
            };
        };
    }
}

#include VG_GFXAPI_HEADER(RasterizerState)

namespace vg::graphics::driver
{
    class RasterizerState : public VG_GFXAPI::RasterizerState
    {
        using super = VG_GFXAPI::RasterizerState;

    public:
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