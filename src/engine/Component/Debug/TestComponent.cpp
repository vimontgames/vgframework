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

        registerPropertyGroupBegin(TestComponent, "Unsigned Int");
        {
            registerProperty(TestComponent, m_u8, "Uint8");
            registerProperty(TestComponent, m_u8_range, "Uint8 Range");
            setPropertyRange(TestComponent, m_u8_range, float2(1, 16));

            registerProperty(TestComponent, m_u16, "Uint16");
            registerProperty(TestComponent, m_u16_range, "Uint16 Range");
            setPropertyRange(TestComponent, m_u16_range, float2(1, 4096));

            registerProperty(TestComponent, m_u32, "Uint32");
            registerProperty(TestComponent, m_u32_range, "Uint32 Range");
            setPropertyRange(TestComponent, m_u32_range, float2(1, 128384));

            registerProperty(TestComponent, m_u64, "Uint64");
            registerProperty(TestComponent, m_u64_range, "Uint64 Range");
            setPropertyRange(TestComponent, m_u64_range, float2(1, 128384UL* 128384UL));
        }
        registerPropertyGroupEnd(TestComponent);

        registerPropertyGroupBegin(TestComponent, "Signed Int");
        {
            registerProperty(TestComponent, m_i8, "Int8");
            registerProperty(TestComponent, m_i8_range, "Int8 Range");
            setPropertyRange(TestComponent, m_i8_range, float2(-16, 15));
                                              
            registerProperty(TestComponent, m_i16, "Int16");
            registerProperty(TestComponent, m_i16_range, "Int16 Range");
            setPropertyRange(TestComponent, m_i16_range, float2(-1000, +1000));
                                              
            registerProperty(TestComponent, m_i32, "Int32");
            registerProperty(TestComponent, m_i32_range, "Int32 Range");
            setPropertyRange(TestComponent, m_i32_range, float2(-100000, +100000));

            registerProperty(TestComponent, m_i64, "Int64");
            registerProperty(TestComponent, m_i64_range, "Int64 Range");
            setPropertyRange(TestComponent, m_i64_range, float2(-10000000, 10000000L));
        }
        registerPropertyGroupEnd(TestComponent);

        registerPropertyGroupBegin(TestComponent, "Float");
        {
            registerProperty(TestComponent, m_float, "Float");
            registerProperty(TestComponent, m_float_range, "Float Range");
            setPropertyRange(TestComponent, m_float_range, float2(1.0, 42.0f));

            registerProperty(TestComponent, m_float2, "Float2");
            registerProperty(TestComponent, m_float2_range, "Float2 Range");
            setPropertyRange(TestComponent, m_float2_range, float2(-42, +42));

            registerProperty(TestComponent, m_float3, "Float3");
            registerProperty(TestComponent, m_float4, "Float4");
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
    void TestComponent::Update(float _dt)
    {
        //super::Update(_dt);
    }
}