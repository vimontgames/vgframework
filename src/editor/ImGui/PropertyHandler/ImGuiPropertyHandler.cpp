#include "editor/Precomp.h"
#include "ImGuiPropertyHandler.h"
#include "core/IGameObject.h"

namespace vg::editor
{
    core::vector<ImGuiPropertyHandler::HandlerInfo> ImGuiPropertyHandler::s_ImGuiHandlers;

    //--------------------------------------------------------------------------------------
    bool ImGuiPropertyHandler::display(const core::IProperty * _prop, core::IObject * _object)
    {
        auto handler = Find(_prop->GetType());
        if (nullptr != handler)
        {
            handler->displayProperty(_prop, _object);
            return true;
        }
        return false;
    }
}
