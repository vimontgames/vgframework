#include "editor/Precomp.h"
#include "ImGuiObjectHandler.h"
#include "core/IObject.h"
#include "editor/Editor_Consts.h"

namespace vg::editor
{
    core::vector<ImGuiObjectHandler::HandlerInfo> ImGuiObjectHandler::s_ImGuiHandlers;

    //--------------------------------------------------------------------------------------
    bool ImGuiObjectHandler::display(core::IObject * _object)
    {
        auto handler = Find(_object->getClassName());
        if (nullptr != handler)
        {
            handler->displayObject(_object);
            return true;
        }
        return false;
    }
}

#include "GameObject/ImguiGameObjectHandler.hpp"
#include "Texture/ImGuiTextureHandler.hpp"