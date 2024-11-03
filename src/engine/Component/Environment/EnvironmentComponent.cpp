#include "engine/Precomp.h"
#include "EnvironmentComponent.h"
#include "core/IBaseScene.h"
#include "core/IGameObject.h"
#include "engine/Engine.h"
#include "renderer/IRenderer.h"
#include "editor/Editor_Consts.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(EnvironmentComponent, "Environment", "Renderer", "Environment settings for scene", editor::style::icon::Environment, 0);

    //--------------------------------------------------------------------------------------
    bool EnvironmentComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEx(EnvironmentComponent, m_environmentColor, "Color", PropertyFlags::Color);
        setPropertyDescription(EnvironmentComponent, m_environmentColor, "Environment color is used as fallback when no environment cubemap is provided");

        registerOptionalPropertyResource(EnvironmentComponent, m_useEnvironmentCubemap, m_environmentCubemap, "Cubemap");
        setPropertyDescription(EnvironmentComponent, m_environmentCubemap, "Environment cubemap to use for ambient diffuse and specular lighting");

        registerProperty(EnvironmentComponent, m_environmentIntensity, "Ambient");
        setPropertyRange(EnvironmentComponent, m_environmentIntensity, float2(0, 10));
        setPropertyDescription(EnvironmentComponent, m_environmentIntensity, "Environment ambient intensity");

        return true;
    }

    //--------------------------------------------------------------------------------------
    EnvironmentComponent::EnvironmentComponent(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {
        m_environmentCubemap.SetParent(this);
    }

    //--------------------------------------------------------------------------------------
    EnvironmentComponent::~EnvironmentComponent()
    {

    }

    //--------------------------------------------------------------------------------------
    void EnvironmentComponent::Update(const Context & _context)
    {
        // Get the world we're rendering to
        if (auto * world = _context.m_gameObject->GetWorld())
        {
            world->SetEnvironmentColor(m_environmentColor);

            if (m_useEnvironmentCubemap)
                world->SetEnvironmentCubemap(m_environmentCubemap.getTexture());
            else
                world->SetEnvironmentCubemap(nullptr);

            world->SetEnvironmentAmbientIntensity(m_environmentIntensity);
        }
    }
}