#include "engine/Precomp.h"
#include "GraphicInstanceComponent.h"
#include "renderer/IGraphicInstance.h"
#include "renderer/IRendererOptions.h"
#include "core/GameObject/GameObject.h"
#include "engine/Engine.h"

using namespace vg::core;
using namespace vg::renderer;

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    core::vector<core::string> getRendererCategoryNames(const IObject * _object, const IProperty * _prop, core::uint _index)
    {
        core::vector<core::string> names;
        names.reserve(enumCount<OutlineCategory>());
        const auto & options = Engine::get()->getRendererOptions();
        for (uint i = 0; i < enumCount<OutlineCategory>(); ++i)
        {
            string name;
            
            if (i < options->GetRendererOutlineCategoryCount()) // TODO hide first OUTLINE_MASK_RESERVED_CATEGORIES categories
                name = options->GetRendererOutlineCategoryName((OutlineCategory)i);
            else
                name = "";

            names.push_back(name);
        }

        return names;
    }

    //--------------------------------------------------------------------------------------
    bool GraphicInstanceComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEnum(GraphicInstanceComponent, renderer::OutlineCategory, m_outlineCategory, "Outline");
        setPropertyNamesCallback(GraphicInstanceComponent, m_outlineCategory, getRendererCategoryNames);

        return true;
    }

    //--------------------------------------------------------------------------------------
    GraphicInstanceComponent::GraphicInstanceComponent(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    GraphicInstanceComponent::~GraphicInstanceComponent()
    {

    }

    //--------------------------------------------------------------------------------------
    void GraphicInstanceComponent::OnLoad()
    {
        if (IGraphicInstance * graphicInstance = GetGraphicInstance())
            graphicInstance->SetOutlineCategory(m_outlineCategory);
    }

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

            const auto objectFlags = go->GetObjectFlags();
            instance->SetObjectFlags(objectFlags, true);
            instance->SetObjectFlags(~objectFlags, false);
            
            const auto objectRuntimeFlags = go->GetObjectRuntimeFlags();
            instance->SetObjectRuntimeFlags(objectRuntimeFlags, true);
            instance->SetObjectRuntimeFlags(~objectRuntimeFlags, false);

            const auto instanceFlags = go->getInstanceFlags();
            instance->SetInstanceFlags(instanceFlags, true);
            instance->SetInstanceFlags(~instanceFlags, false);

            const auto instanceRuntimeFlags = go->getInstanceRuntimeFlags();
            instance->SetInstanceRuntimeFlags(instanceRuntimeFlags, true);
            instance->SetInstanceRuntimeFlags(~instanceRuntimeFlags, false);

            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    void GraphicInstanceComponent::OnPropertyChanged(core::IObject * _object, const core::IProperty & _prop, bool _notifyParent)
    {
        super::OnPropertyChanged(_object, _prop, _notifyParent);

        if (!strcmp(_prop.GetName(), "m_outlineCategory"))
        {
            if (IGraphicInstance * graphicInstance = GetGraphicInstance())
                graphicInstance->SetOutlineCategory(m_outlineCategory);
        }

        RefreshGraphicInstance();
    }
}