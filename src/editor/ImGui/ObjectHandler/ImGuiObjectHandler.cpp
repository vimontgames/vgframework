#include "editor/Precomp.h"
#include "ImGuiObjectHandler.h"
#include "core/IObject.h"
#include "editor/Editor_Consts.h"

namespace vg::editor
{
    core::vector<ImGuiObjectHandler::HandlerInfo> ImGuiObjectHandler::s_ImGuiHandlers;

    //--------------------------------------------------------------------------------------
    bool ImGuiObjectHandler::display(core::IObject * _object, ObjectContext & _objectContext)
    {
        auto handler = Find(_object->GetClassName());
        if (nullptr != handler)
        {
            handler->displayObject(_object, _objectContext);
            return true;
        }
        return false;
    }
}

#include "GameObject/ImGuiGameObjectHandler.hpp"
#include "Texture/ImGuiTextureHandler.hpp"
#include "Material/ImGuiMaterialModelTypeHandler.hpp"

#include "ImGuiResourceListHandler.hpp"
#include "Material/ImGuiMaterialResourceListHandler.hpp"
#include "Animation/ImGuiAnimationResourceListHandler.hpp"
#include "Animation/ImGuiSkeletonObjectHandler.hpp"