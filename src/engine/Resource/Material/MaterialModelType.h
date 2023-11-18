#pragma once

#include "core/Object/Object.h"

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    // Wrapper object for shader name to customize display
    //--------------------------------------------------------------------------------------
    class MaterialModelType : public core::Object
    {
    public:
        VG_CLASS_DECL(MaterialModelType, core::Object);
        MaterialModelType(const core::string & _name = "", core::IObject * _parent = nullptr) : core::Object(_name, _parent) {}
        core::string m_shader;
    };
}