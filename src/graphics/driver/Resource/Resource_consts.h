#pragma once

namespace vg::graphics::driver
{
    enum class Usage : core::u8
    {
        Default = 0,
        Upload,
        //Static,
        //Staging,
        Dynamic
    };

    enum class CPUAccessFlags : core::u8
    {
        None                = 0x00,
        Read                = 0x01,
        Write               = 0x02,
        ReadWrite           = 0x03
    };

    enum class BindFlags : core::u16
    {
        None				= 0x0000,
        ConstantBuffer      = 0x0001,
        ShaderResource      = 0x0002,
        IndexBuffer			= 0x0004,
        VertexBuffer		= 0x0008,
        UnorderedAccess		= 0x0010,
        IndirectArgsBuffer	= 0x0020
    };

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

        inline bool isDynamicResource() const   { return core::u16(Usage::Dynamic) & core::u16(m_usage); }

        inline bool isShaderResource() const    { return core::u16(BindFlags::ShaderResource) & core::u16(m_bindFlags); }
    };
}