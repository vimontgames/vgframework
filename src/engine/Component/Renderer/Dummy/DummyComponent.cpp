#include "engine/Precomp.h"
#include "DummyComponent.h"
#include "editor/Editor_Consts.h"
#include "renderer/IDebugDraw.h"
#include "engine/Engine.h"
#include "renderer/IRenderer.h"
#include "core/IGameObject.h"
#include "corE/Math/Math.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(DummyComponent, "Dummy", "Renderer", "A dummy component is an helper object only visible in toolmode", editor::style::icon::Unchecked, 0);

    //--------------------------------------------------------------------------------------
    bool DummyComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    DummyComponent::DummyComponent(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {
        EnableUpdateFlags(UpdateFlags::Update, false);
        EnableUpdateFlags(UpdateFlags::ToolUpdate, true);
    }

    //--------------------------------------------------------------------------------------
    DummyComponent::~DummyComponent()
    {

    }

    //--------------------------------------------------------------------------------------
    void DummyComponent::ToolUpdate(const Context & _context)
    {
        renderer::IDebugDraw * debugDraw = Engine::get()->GetRenderer()->GetDebugDraw();

        debugDraw->AddWireframeCube(_context.m_world, float3(-0.5f, -0.5f, -0.5f), float3(0.5f, 0.5f, 0.5f), getU32Color(_context.m_gameObject->getColor()), _context.m_gameObject->GetGlobalMatrix());
    }
}