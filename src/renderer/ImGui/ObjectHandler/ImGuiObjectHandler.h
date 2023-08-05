#pragma once

#include "renderer/Imgui/ImGuiCustomHandler.h"

namespace vg::renderer
{
    class ImGuiObjectHandler : public ImGuiCustomHandler<core::string, ImGuiObjectHandler>
    {
    public:
        static bool display(core::IObject * _object);
        virtual void displayObject(core::IObject * _object) = 0;
    };

    #define VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(className, handler) VG_AUTO_REGISTER_IMGUI_CUSTOM_HANDLER(className, handler, core::string, ImGuiObjectHandler)
}