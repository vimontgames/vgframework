#pragma once

#include "engine/Component/Mesh/MaterialResourceList.h"
#include "core/File/File.h"

using namespace vg::core;

namespace vg::editor
{
    class ImGuiMaterialResourceListHandler : public ImGuiResourceListHandler
    {
    public:
        //--------------------------------------------------------------------------------------
        void displayObject(IObject * _object, ObjectContext & _objectContext) final
        {
            displayResourceList(_object, "Material", "m_materialResources");
        }
    };

    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(MaterialResourceList, ImGuiMaterialResourceListHandler);
}



