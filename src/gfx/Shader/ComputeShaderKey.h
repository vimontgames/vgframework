#pragma once

namespace vg::gfx
{
    class ComputeShaderKey
    {
    public:
        using File = core::u8;
        using EntryPoint = core::u8;

        using CS = EntryPoint;

        using Flags = core::u16;

        ComputeShaderKey();
        ComputeShaderKey(const core::string & _file, const core::string & _technique);
        ~ComputeShaderKey();

        void clear();
        void init(const core::string & _file, const core::string & _technique);

        inline void setFlags(core::uint _index, bool _enable = true)
        {
            const ComputeShaderKey::Flags value = (ComputeShaderKey::Flags)(1 << _index);

            if (_enable)
                flags |= value;
            else
                flags &= value;
        }

        union
        {
            struct
            {
                File file;
                CS cs;
                Flags flags;
            };
            core::u32 bits;
        };

        bool operator == (const ComputeShaderKey & _other) const
        {
            return _other.bits == bits;
        }

        bool operator != (const ComputeShaderKey & _other) const
        {
            return _other.bits != bits;
        }

        struct hash
        {
            inline core::size_t operator() (const ComputeShaderKey & _this) const
            {
                VG_STATIC_ASSERT(sizeof(_this) == sizeof(core::u32), "Invalid sizeof in hash function");
                return core::hash<core::u32>()(_this.bits);
            }
        };
    };
}