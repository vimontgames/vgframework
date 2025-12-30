#include "engine/Precomp.h"
#include "GraphicInstanceComponent.h"
#include "renderer/IGraphicInstance.h"
#include "core/GameObject/GameObject.h"

using namespace vg::core;
using namespace vg::renderer;

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    void GraphicInstanceComponent::OnEnable()
    {
        registerGraphicInstance();
    }

    //--------------------------------------------------------------------------------------
    void GraphicInstanceComponent::OnDisable()
    {
        unregisterGraphicInstance();
    }

    //--------------------------------------------------------------------------------------
    bool GraphicInstanceComponent::registerGraphicInstance()
    {
        if (!m_registered)
        {
            if (auto * instance = GetGraphicInstance())
            {
                if (getGameObject()->addGraphicInstance(instance))
                {
                    m_registered = true;
                    return true;
                }
            }
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool GraphicInstanceComponent::unregisterGraphicInstance()
    {
        if (m_registered)
        {
            if (auto * instance = GetGraphicInstance())
            {
                if (getGameObject()->removeGraphicInstance(instance))
                {
                    m_registered = false;
                    return true;
                }
            }
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    // Propagate GameObject's instance flags to GraphicInstance's instance flags
    //--------------------------------------------------------------------------------------
    bool GraphicInstanceComponent::UpdateFlagsFromGameObject()
    {
        if (IGraphicInstance * instance = GetGraphicInstance())
        {
            GameObject * go = getGameObject();
            go->recomputeUpdateFlags();

            const auto flags = go->getInstanceFlags();

            instance->SetInstanceFlags(flags, true);
            instance->SetInstanceFlags(~flags, false);

            const auto runtimeFlags = go->getInstanceRuntimeFlags();
            instance->SetInstanceRuntimeFlags(runtimeFlags, true);
            instance->SetInstanceRuntimeFlags(~runtimeFlags, false);

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