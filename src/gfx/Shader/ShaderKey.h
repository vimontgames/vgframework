#pragma once

namespace vg::gfx
{
    class ShaderKey
    {
    public:
        using File = core::u8;
        using EntryPoint = core::u8;

        using VS = EntryPoint;
        using HS = EntryPoint;
        using DS = EntryPoint;
        using GS = EntryPoint;
        using PS = EntryPoint;

        using Flags = core::u16;

        ShaderKey();
        ShaderKey(const core::string & _file, const core::string & _technique);
        ~ShaderKey();

        void clear();
        void init(const core::string & _file, const core::string & _technique);

        inline void setFlags(core::uint _index, bool _enable = true)
        {
            const ShaderKey::Flags value = (ShaderKey::Flags)(1 << _index);

            if (_enable)
                flags |= value;
            else
                flags &= ~value;
        }

        union
        {
            struct
            {
                File file;
                VS vs;
                HS hs;
                DS ds;
                GS gs;
                PS ps;
                Flags flags;
            };
            core::u64 bits;
        };

        bool operator == (const ShaderKey & _other) const
        {
            return _other.bits == bits;
        }

        bool operator != (const ShaderKey & _other) const
        {
            return _other.bits != bits;
        }

        struct hash
        {
            inline core::size_t operator() (const ShaderKey & _this) const
            {
                VG_STATIC_ASSERT(sizeof(_this) == sizeof(core::u64), "Invalid sizeof in hash function");
                return core::hash<core::u64>()(_this.bits);
            }
        };
    };
}