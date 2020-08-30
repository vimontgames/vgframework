#include "RootSignatureDesc.h"

namespace vg::graphics::driver
{
    //--------------------------------------------------------------------------------------
    bool RootSignatureDesc::Table::isEmpty() const
    {
        return m_descriptors.size() == 0;
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
        m_constants.push_back({ _stages, _offset, _count });
        return (uint)(m_constants.size() - 1);
    }
}