#pragma once

namespace vg::gfx
{
    class RayTracingShaderKey
    {
    public:
        using File       = core::u8;
        using EntryPoint = core::u8;

        using RG = EntryPoint; 
        using AH = EntryPoint; 
        using CH = EntryPoint; 
        using MI = EntryPoint; 
        using IS = EntryPoint; 

        using Flags = core::u16;

        RayTracingShaderKey();
        RayTracingShaderKey(const core::string & _file, const core::string & _technique);
        ~RayTracingShaderKey();

        void clear();
        void init(const core::string & _file, const core::string & _technique);

        inline void setFlags(core::uint _index, bool _enable = true)
        {
            const RayTracingShaderKey::Flags value = (RayTracingShaderKey::Flags)(1 << _index);

            if (_enable)
                flags |= value;
            else
                flags &= value;
        }

        inline EntryPoint getEntryPointIndex(RayTracingStage _stage) const
        {
            switch (_stage)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(_stage);

                case RayTracingStage::RayGen:
                    return rg;

                case RayTracingStage::AnyHit:
                    return ah;

                case RayTracingStage::ClosestHit:
                    return ch;

                case RayTracingStage::Miss:
                    return mi;

                case RayTracingStage::Intersection:
                    return is;                        
            }
        }

        bool operator == (const RayTracingShaderKey & _other) const
        {
            return _other.bits == bits;
        }

        bool operator != (const RayTracingShaderKey & _other) const
        {
            return _other.bits != bits;
        }

        bool operator < (const RayTracingShaderKey & _other) const
        {
            return _other.bits < bits;
        }

        union
        {
            struct
            {
                File    file;
                RG      rg;
                AH      ah;
                CH      ch;
                MI      mi;
                IS      is;
                Flags   flags;
            };
            core::u64 bits = 0x0;
        };

        struct hash
        {
            inline core::size_t operator() (const RayTracingShaderKey & _this) const
            {
                VG_STATIC_ASSERT(sizeof(_this) == sizeof(core::u64), "Invalid sizeof in hash function");
                return core::hash<core::u64>()(_this.bits);
            }
        };
    };
}