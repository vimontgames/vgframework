#include "engine/Precomp.h"
#include "DummyComponent.h"
#include "editor/Editor_Consts.h"
#include "renderer/IDebugDraw.h"
#include "engine/Engine.h"
#include "engine/EngineOptions.h"
#include "renderer/IRenderer.h"
#include "core/IGameObject.h"
#include "corE/Math/Math.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(DummyComponent, "Dummy", "Renderer", "A dummy component is an helper object only visible in toolmode", editor::style::icon::Unchecked, getPriority(ComponentGroup::Render, ComponentPriority::Late));

    //--------------------------------------------------------------------------------------
    bool DummyComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEnum(DummyComponent, DummyType, m_dummyType, "Type");

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
        if (EngineOptions::get()->isDummyTypeVisible(_context.m_world, m_dummyType))
        {
            renderer::IDebugDraw * debugDraw = Engine::get()->GetRenderer()->GetDebugDraw();

            switch (m_dummyType)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(m_dummyType);
                    break;

                case DummyType::Box:
                    debugDraw->AddWireframeCube(_context.m_world, float3(-0.5f, -0.5f, -0.5f), float3(0.5f, 0.5f, 0.5f), getU32Color(_context.m_gameObject->getColor()), _context.m_gameObject->GetGlobalMatrix());
                    break;

                case DummyType::Sphere:
                    debugDraw->AddWireframeSphere(_context.m_world, 0.5f, getU32Color(_context.m_gameObject->getColor()), _context.m_gameObject->GetGlobalMatrix());
                    break;
            }
        }
    }
}