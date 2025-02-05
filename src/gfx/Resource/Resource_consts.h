#pragma once

namespace vg::gfx
{
    vg_enum_class(vg::gfx, Usage, core::u8,
        Default = 0,
        Upload,
        Staging
    );

    vg_enum_class(vg::gfx, CPUAccessFlags, core::u8,
        None                = 0x00,
        Read                = 0x01,
        Write               = 0x02,
        ReadWrite           = 0x03
    );

    vg_enum_class(vg::gfx, BindFlags, core::u16,
        None				            = 0x0000,
        ConstantBuffer                  = 0x0001,
        ShaderResource                  = 0x0002,
        IndexBuffer			            = 0x0004,
        VertexBuffer		            = 0x0008,
        UnorderedAccess		            = 0x0010,
        IndirectArgsBuffer	            = 0x0020,
		RaytracingAccelerationStruct    = 0x0040
    );

    class ResourceDesc
    {
    public:
        ResourceDesc(Usage _usage, BindFlags _bindFlags, CPUAccessFlags _cpuAccessFlags) : 
            bits(0x0)
        {
            m_usage = _usage;
            m_bindFlags = _bindFlags;
            m_cpuAccessFlags = _cpuAccessFlags;
        }

        union
        {
            struct
            {
                Usage           m_usage          : 2;
                BindFlags       m_bindFlags      : 9;
                CPUAccessFlags  m_cpuAccessFlags : 2;
            };
            core::u16 bits;
        };

		inline bool testBindFlags(BindFlags _flags) const	{ return std::underlying_type<BindFlags>::type(_flags) & std::underlying_type<BindFlags>::type(m_bindFlags); }

        VG_INLINE bool operator != (const ResourceDesc & _other) const { return bits != _other.bits; }
    };

    struct Map
    {
        void * VG_RESTRICT data = nullptr;
        core::u32 rowPitch = 0;
        core::u32 slicePitch = 0;
    };

    struct Range
    {
        core::u32 begin = 0;
        core::u32 end = 0;
    };
}