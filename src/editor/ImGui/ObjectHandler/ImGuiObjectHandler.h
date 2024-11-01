#pragma once

#include "editor/ImGui/ImGuiCustomHandler.h"

namespace vg::editor
{
    struct ObjectContext;
    struct PropertyContext;

    class ImGuiObjectHandler : public ImGuiCustomHandler<core::string, ImGuiObjectHandler>
    {
    public:
        virtual ~ImGuiObjectHandler() = default;

        // Return 'true' if object is edited
        virtual bool displayObject(core::IObject * _object, ObjectContext & _objectContext, const PropertyContext * _propContext = nullptr) = 0;
    };

    #define VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(className, handler) VG_AUTO_REGISTER_IMGUI_CUSTOM_HANDLER(className, handler, core::string, ImGuiObjectHandler)
}