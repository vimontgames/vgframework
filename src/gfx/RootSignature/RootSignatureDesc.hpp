#include "RootSignatureDesc.h"

namespace vg::gfx
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
    uint RootSignatureDesc::addRootConstants(ShaderStageFlags _stages, u8 _binding, u16 _register, u16 _count)
    {
        m_pushConstants.push_back({ _stages, _binding, _register, _count });
        return (uint)(m_pushConstants.size() - 1);
    }

    //--------------------------------------------------------------------------------------
    // RayTracing RootSignature Layout:
    //   m_rayTracingOutputUAVCount               : [0..N[
    //   m_rayTracingAccellerationStructureCount  : [N..M[
    //--------------------------------------------------------------------------------------
    core::uint RootSignatureDesc::addRayTracingOutputUAV(core::uint _count)
    {
        VG_ASSERT(m_rayTracingOutputUAVCount == 0);
        m_rayTracingOutputUAVCount = _count;

        return 0;
    }

    //--------------------------------------------------------------------------------------
    core::uint RootSignatureDesc::addRayTracingAccelerationStructure(core::uint _count)
    {
        VG_ASSERT(m_rayTracingAccellerationStructureCount == 0);
        m_rayTracingAccellerationStructureCount = _count;

        return m_rayTracingOutputUAVCount;
    }
}