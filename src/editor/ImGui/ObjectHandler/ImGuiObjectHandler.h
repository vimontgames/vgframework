#pragma once

#include "editor/ImGui/ImGuiCustomHandler.h"

namespace vg::editor
{
    struct ObjectContext;

    class ImGuiObjectHandler : public ImGuiCustomHandler<core::string, ImGuiObjectHandler>
    {
    public:
        virtual ~ImGuiObjectHandler() = default;
        static bool display(core::IObject * _object, ObjectContext & _objectContext);
        virtual void displayObject(core::IObject * _object, ObjectContext & _objectContext) = 0;
    };

    #define VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(className, handler) VG_AUTO_REGISTER_IMGUI_CUSTOM_HANDLER(className, handler, core::string, ImGuiObjectHandler)
}