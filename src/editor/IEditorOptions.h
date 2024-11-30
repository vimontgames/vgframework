#pragma once
#include "core/Options/Options.h"
#include "editor/Editor_Consts.h"

namespace vg::renderer
{
    class ICameraSettings;
}

namespace vg::editor
{
    class IEditorOptions : public core::Options
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IEditorOptions, core::Options);

        virtual renderer::ICameraSettings * GetCameraSettings() const = 0;
    };
}