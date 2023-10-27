#pragma once

#include "gfx/Shader/Shader_consts.h"
#include "RootSignatureTableDesc.h"

namespace vg::gfx
{
    class RootSignatureDesc
    {
    public:
        struct PushConstantParams
        {
            inline bool operator == (const PushConstantParams & _other) const
            {
                return m_stages   == _other.m_stages
                    && m_binding  == _other.m_binding
                    && m_register == _other.m_register
                    && m_count    == _other.m_count; 
            }
            inline bool operator != (const PushConstantParams & _other) const { return !(operator == (_other)); }

            ShaderStageFlags    m_stages = (ShaderStageFlags)0;
            core::u8            m_binding = 0;
            core::u16           m_register = 0;
            core::u16           m_count = 0;
        
        };

        core::uint addRootConstants(ShaderStageFlags _stages, core::u8 _binding, core::u16 _register, core::u16 _count);
        core::uint addTable(const RootSignatureTableDesc & _table);

        core::uint addRayTracingOutputUAV(core::uint _count = 1);
        core::uint addRayTracingAccelerationStructure(core::uint _count = 1);

        const core::vector<PushConstantParams> & getRootConstants() const { return m_pushConstants; }
        const core::vector<RootSignatureTableDesc> & getTables() const { return m_tables; }

        bool operator == (const RootSignatureDesc & _other) const;
        inline bool operator != (const RootSignatureDesc & _other) const { return !(operator == (_other)); }

    //private:
        core::vector<PushConstantParams>        m_pushConstants;
        core::vector<RootSignatureTableDesc>    m_tables;
        core::uint                              m_rayTracingOutputUAVCount = 0;
        core::uint                              m_rayTracingAccellerationStructureCount = 0;
    };
}