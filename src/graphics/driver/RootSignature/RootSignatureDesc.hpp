#include "RootSignatureDesc.h"

namespace vg::graphics::driver
{
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
    uint RootSignatureDesc::addTable(const RootSignatureTableDesc & _table)
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
}