#pragma once

#include "engine/Component/Animation/AnimationResourceList.h"
#include "core/File/File.h"

using namespace vg::core;

namespace vg::editor
{
    class ImGuiAnimationListHandler : public ImGuiResourceListHandler
    {
    public:
        //--------------------------------------------------------------------------------------
        void displayObject(IObject * _object) final
        {
            displayResourceList(_object, "Animation", "m_animationResources");
        }
    };

    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(AnimationResourceList, ImGuiAnimationListHandler);
}



