#pragma once

#include "Graphics/driver/Shader/Shader_consts.h"
#include "RootSignatureTableDesc.h"

namespace vg::graphics::driver
{
    class RootSignatureDesc
    {
    public:
        struct PushConstantParams
        {
            inline bool operator == (const PushConstantParams & _other) const
            {
                return m_stages == _other.m_stages
                    && m_start  == _other.m_start
                    && m_count  == _other.m_count
                    && m_space  == _other.m_space; 
            }
            inline bool operator != (const PushConstantParams & _other) const { return !(operator == (_other)); }

            ShaderStageFlags    m_stages = (ShaderStageFlags)0;
            core::u8            m_start = 0;
            core::u8            m_count = 0;
            core::u8            m_space = 0;
        };

        core::uint addRootConstants(ShaderStageFlags _stages, core::u8 _offset, core::u8 _count);
        core::uint addTable(const RootSignatureTableDesc & _table);

        const core::vector<PushConstantParams> & getRootConstants() const { return m_pushConstants; }
        const core::vector<RootSignatureTableDesc> & getTables() const { return m_tables; }

        bool operator == (const RootSignatureDesc & _other) const;
        inline bool operator != (const RootSignatureDesc & _other) const { return !(operator == (_other)); }

    private:
        core::vector<PushConstantParams>        m_pushConstants;
        core::vector<RootSignatureTableDesc>    m_tables;
    };
}