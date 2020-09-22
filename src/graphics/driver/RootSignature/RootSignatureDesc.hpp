#include "RootSignatureDesc.h"

namespace vg::graphics::driver
{
    //--------------------------------------------------------------------------------------
    // RootSignatureDesc
    //--------------------------------------------------------------------------------------
    bool RootSignatureDesc::operator == (const RootSignatureDesc & _other) const
    {
        const core::uint pushConstantsCount = (uint)m_pushConstants.size();
        if (pushConstantsCount != _other.m_pushConstants.size())
            return false;

        const core::uint tableCount = (uint)m_tables.size();
        if (tableCount != _other.m_tables.size())
            return false;

        for (core::uint i = 0; i < pushConstantsCount; ++i)
            if (m_pushConstants[i] != _other.m_pushConstants[i])
                return false;

        for (core::uint i = 0; i < tableCount; ++i)
            if (m_tables[i] != _other.m_tables[i])
                return false;

        return true;
    }
    
    //--------------------------------------------------------------------------------------
    uint RootSignatureDesc::addTable(const Table & _table)
    {
        m_tables.push_back(_table);
        return (uint)(m_tables.size() - 1);
    }

    //--------------------------------------------------------------------------------------
    uint RootSignatureDesc::addRootConstants(ShaderStageFlags _stages, u8 _offset, u8 _count)
    {
        m_pushConstants.push_back({ _stages, _offset, _count });
        return (uint)(m_pushConstants.size() - 1);
    }

    //--------------------------------------------------------------------------------------
    // RootSignatureDesc::Table
    //--------------------------------------------------------------------------------------
    bool RootSignatureDesc::Table::isEmpty() const
    {
        return 0 == m_descriptors.size();
    }

    //--------------------------------------------------------------------------------------
    bool RootSignatureDesc::Table::operator == (const Table & _other) const
    {
        if (m_stages != _other.m_stages)
            return false;

        const uint descriptorCount = (uint)m_descriptors.size();
        if (descriptorCount != _other.m_descriptors.size())
            return false;

        for (uint i = 0; i < descriptorCount; ++i)
            if (m_descriptors[i] != _other.m_descriptors[i])
                return false;

        return true;
    }

    //--------------------------------------------------------------------------------------
    // RootSignatureDesc::Table
    //--------------------------------------------------------------------------------------
    void RootSignatureDesc::Table::addTextures(core::u8 _register, core::u16 _count, core::u8 _space, core::u16 _offset)
    {
        RootSignatureDesc::Table::Descriptor descritor;
        descritor.m_type = Descriptor::Type::Texture;
        descritor.texture = Descriptor::Texture(_register, _count, _space, _offset);
        m_descriptors.push_back(descritor);
    }

    //--------------------------------------------------------------------------------------
    const core::vector<RootSignatureDesc::Table::Descriptor> & RootSignatureDesc::Table::getDescriptors() const
    {
        return m_descriptors;
    }
}