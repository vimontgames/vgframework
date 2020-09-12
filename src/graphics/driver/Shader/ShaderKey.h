#pragma once

namespace vg::graphics::driver
{
    class ShaderKey
    {
    public:
        using File  = core::u8;
        using VS    = core::u8;
        using HS    = core::u8;
        using DS    = core::u8;
        using GS    = core::u8;
        using PS    = core::u8;
        using Flags = core::u16;

        ShaderKey();
        ShaderKey(const core::string & _file, const core::string & _technique);
        ~ShaderKey();

        void clear();
        void init(const core::string & _file, const core::string & _technique);

        union
        {
            struct
            {
                File    file;
                VS      vs;
                HS      hs;
                DS      ds;
                GS      gs;
                PS      ps;
                Flags   flags;
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
            inline core::size_t operator() (const driver::ShaderKey & _this) const
            {
                VG_STATIC_ASSERT(sizeof(_this) == sizeof(core::u64), "Invalid sizeof in hash function");
                return core::hash<core::u64>()(_this.bits);
            }
        };
    };
}