#pragma once

#include "ShaderKey.h"

namespace vg::graphics::driver
{
    class HLSLDesc
    {
    public:
        struct Technique
        {
            core::string    name;
            ShaderKey::VS   vs = (ShaderKey::VS) - 1;
            ShaderKey::HS   hs = (ShaderKey::HS) - 1;
            ShaderKey::DS   ds = (ShaderKey::DS) - 1;
            ShaderKey::GS   gs = (ShaderKey::GS) - 1;
            ShaderKey::PS   ps = (ShaderKey::PS) - 1;
            core::u16       flags = 0x0000;

            bool operator == (const Technique & _other) const
            {
                return vs == _other.vs && hs == _other.hs && ds == _other.ds && gs == _other.gs && ps == _other.ps && !name.compare(_other.name);
            }
        };

        ~HLSLDesc(); // release all compiled shaders

        void setFile(const core::string & _file);
        const core::string & getFile() const;

        ShaderKey::VS addVS(const core::string & _vsEntryPoint);
        ShaderKey::PS addPS(const core::string & _psEntryPoint);

        Shader * getVS(ShaderKey::VS _vs);
        Shader * getPS(ShaderKey::PS _ps);

        core::uint addShader(ShaderStage _stage, const core::string & _entryPoint);
        Shader * getShader(ShaderStage _stage, core::uint _index);

        Technique & addTechnique(const core::string & _name);
        const core::vector<Technique> & getTechniques() const;

    private:
        core::string                                        m_file;
        core::vector<core::pair<core::string, Shader *>>    m_shader[core::enumCount<ShaderStage>()];
        core::vector<Technique>                             m_techniques;
    };
}