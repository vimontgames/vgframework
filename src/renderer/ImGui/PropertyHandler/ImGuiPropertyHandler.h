#pragma once

#include "renderer/Imgui/ImGuiCustomHandler.h"

namespace vg::renderer
{
    class ImGuiPropertyHandler : public ImGuiCustomHandler<core::IProperty::Type,ImGuiPropertyHandler>
    {
    public:
        static bool display(const core::IProperty * _prop, core::IObject * _object);
        virtual void displayProperty(const core::IProperty * _prop, core::IObject * _object) = 0;
    };

    #define VG_AUTO_REGISTER_IMGUI_PROPERTY_HANDLER(className, handler) VG_AUTO_REGISTER_IMGUI_CUSTOM_HANDLER(className, handler, IProperty::Type, ImGuiPropertyHandler)
}