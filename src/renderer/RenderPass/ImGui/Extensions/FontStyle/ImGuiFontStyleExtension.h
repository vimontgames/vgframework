#pragma once

#include "editor/ImGui/ImGui.h"
#include "core/Types/Types.h"
#include "core/Core.h"

namespace vg::core
{
    class IObject;
}

namespace vg::renderer
{
    enum class Font : vg::core::u8;
    enum class Style : vg::core::u8;
}

namespace ImGui
{
    //void PushFont(vg::renderer::Font _font);
    //void PopFont();
    //
    //void PushStyle(vg::renderer::Style _style);
    //void PopStyle();
}