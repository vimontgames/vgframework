#include "engine/Precomp.h"
#include "GraphicInstanceComponent.h"
#include "renderer/IGraphicInstance.h"
#include "core/IGameObject.h"

using namespace vg::core;
using namespace vg::renderer;

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    // Propagate GameObject's instance flags to GraphicInstance's instance flags
    //--------------------------------------------------------------------------------------
    bool GraphicInstanceComponent::UpdateFlagsFromGameObject()
    {
        if (IGraphicInstance * instance = GetGraphicInstance())
        {
            const IGameObject * go = GetGameObject();
            const auto flags = go->getInstanceFlags();

            instance->SetInstanceFlags(flags, true);
            instance->SetInstanceFlags(~flags, false);

            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    void GraphicInstanceComponent::OnPropertyChanged(core::IObject * _object, const core::IProperty & _prop, bool _notifyParent)
    {
        super::OnPropertyChanged(_object, _prop, _notifyParent);
        RefreshGraphicInstance();
    }
}