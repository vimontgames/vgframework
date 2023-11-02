#pragma once

#include "gfx/Shader/HLSLDesc.h"

namespace vg::gfx
{
    class DefaultHLSLDesc : public HLSLDesc
    {
    public:
        enum Flags : ShaderKey::Flags
        {
            ZOnly    = 0,
            Editor   = 14,
            Toolmode = 15
        };

        DefaultHLSLDesc()
        {
            setFile("default/default.hlsl");

            addFlag(ZOnly, ShaderStageFlags::VS | ShaderStageFlags::PS, "_ZONLY");
            addFlag(Editor, ShaderStageFlags::VS | ShaderStageFlags::PS, "_EDITOR");
            addFlag(Toolmode, ShaderStageFlags::VS | ShaderStageFlags::PS, "_TOOLMODE");

            auto & zonly = addTechnique("ZOnly");
            {
                zonly.vs = addVS("VS_Forward");
                zonly.ps = addPS("PS_Forward");
                zonly.addFlag(Flags::ZOnly);
            }

            auto & forward = addTechnique("Forward");
            {
                forward.vs = addVS("VS_Forward");
                forward.ps = addPS("PS_Forward");
            }

            auto & editor = addTechnique("Editor");
            {
                editor.vs = addVS("VS_Forward");
                editor.ps = addPS("PS_Forward");
                editor.addFlag(Flags::Editor);
            }
        }
    };
}