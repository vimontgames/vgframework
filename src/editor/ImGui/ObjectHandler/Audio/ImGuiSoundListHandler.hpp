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
        bool displayObject(IObject * _object, ObjectContext & _objectContext) final
        {
            return displayResourceList(_object, "Sound", "m_resources");
        }
    };

    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(SoundResourceList, ImGuiSoundListHandler);
}



