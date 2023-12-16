#pragma once
#include "core/Options/Options.h"
#include "editor/Editor_Consts.h"

namespace vg::editor
{
    class IEditorOptions : public core::Options
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IEditorOptions, core::Options);
    };
}