#pragma once

#include "engine/Component/Animation/AnimationResourceList.h"
#include "core/File/File.h"

using namespace vg::core;

namespace vg::editor
{
    class ImGuiSoundListHandler : public ImGuiResourceListHandler
    {
    public:
        //--------------------------------------------------------------------------------------
        bool displayObject(IObject * _object, ObjectContext & _objectContext, const PropertyContext * _propContext) final override
        {
            return displayResourceList(_object, "Sound", "m_resources", _propContext);
        }
    };

    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(SoundResourceList, ImGuiSoundListHandler);
}



