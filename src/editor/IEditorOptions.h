#pragma once
#include "core/Object/Object.h"
#include "editor/Editor_Consts.h"

namespace vg::editor
{
    class IEditorOptions : public core::Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IEditorOptions, core::Object);
    };
}