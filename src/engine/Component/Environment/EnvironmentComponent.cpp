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

        return true;
    }

    //--------------------------------------------------------------------------------------
    EnvironmentComponent::EnvironmentComponent(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {

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
        }
    }
}