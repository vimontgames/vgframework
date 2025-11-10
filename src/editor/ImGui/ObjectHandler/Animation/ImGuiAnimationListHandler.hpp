#pragma once

#include "engine/Component/Engine/Animation/AnimationResourceList.h"
#include "core/File/File.h"

using namespace vg::core;

namespace vg::editor
{
    class ImGuiAnimationListHandler : public ImGuiResourceListHandler
    {
    public:
        //--------------------------------------------------------------------------------------
        bool displayObject(IObject * _object, ObjectContext & _objectContext, const PropertyContext * _propContext) final override
        {
            return displayResourceList(_object, "Animation", "m_resources", _propContext);
        }
    };

    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(AnimationResourceList, ImGuiAnimationListHandler);
}



