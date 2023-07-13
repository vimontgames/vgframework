#include "graphics/renderer/Precomp.h"
#include "ImGuiPropertyHandler.h"
#include "core/IGameObject.h"

namespace vg::graphics::renderer
{
    core::vector<ImGuiPropertyHandler::HandlerInfo> ImGuiPropertyHandler::s_ImGuiHandlers;

    //--------------------------------------------------------------------------------------
    bool ImGuiPropertyHandler::display(const core::IProperty * _prop, core::IObject * _object)
    {
        auto handler = Find(_prop->getType());
        if (nullptr != handler)
        {
            handler->displayProperty(_prop, _object);
            return true;
        }
        return false;
    }
}
