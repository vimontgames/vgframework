#include "engine/Precomp.h"
#include "TestComponent.h"

using namespace vg::core;

#include "editor/Editor_Consts.h"

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(TestComponent, "Test", "Debug", "Test Serializaion", editor::style::icon::Error, 1);

    //--------------------------------------------------------------------------------------
    bool TestComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyGroupBegin(TestComponent, "Integer");
        {
            registerProperty(TestComponent, m_u8, "Uint8 (1)");
            registerProperty(TestComponent, m_u8_range, "Uint8 (2)");
            setPropertyRange(TestComponent, m_u8_range, float2(1, 16));

            registerProperty(TestComponent, m_u16, "Uint16 (1)");
            registerProperty(TestComponent, m_u16_range, "Uint16 (2)");
            setPropertyRange(TestComponent, m_u16_range, float2(1, 4096));

            registerProperty(TestComponent, m_u32, "Uint32 (1)");
            registerProperty(TestComponent, m_u32_range, "Uint32 (2)");
            setPropertyRange(TestComponent, m_u32_range, float2(1, 128384));
        }
        registerPropertyGroupEnd(TestComponent);

        return true;
    }

    //--------------------------------------------------------------------------------------
    TestComponent::TestComponent(const core::string & _name, core::IObject * _parent) :
        core::Component(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    TestComponent::~TestComponent()
    {

    }

    //--------------------------------------------------------------------------------------
    void TestComponent::OnPropertyChanged(core::IObject * _object, const core::IProperty & _prop, bool _notifyParent)
    {
        super::OnPropertyChanged(_object, _prop, _notifyParent);
    }

    //--------------------------------------------------------------------------------------
    void TestComponent::Update(float _dt)
    {
        //super::Update(_dt);
    }
}