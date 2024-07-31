#include "engine/Precomp.h"
#include "TestComponent.h"

using namespace vg::core;

#include "editor/Editor_Consts.h"

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(TestComponent, "Test", "Debug", "Test Serialization", editor::style::icon::Error, 1);

    //--------------------------------------------------------------------------------------
    bool TestComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyGroupBegin(TestComponent, "Unsigned Int");
        {
            registerProperty(TestComponent, m_u8, "Uint8");
            registerProperty(TestComponent, m_u16, "Uint16");
            registerProperty(TestComponent, m_u32, "Uint32");
            registerProperty(TestComponent, m_u64, "Uint64");
        }
        registerPropertyGroupEnd(TestComponent);

        registerPropertyGroupBegin(TestComponent, "Signed Int");
        {
            registerProperty(TestComponent, m_i8, "Int8");
            registerProperty(TestComponent, m_i16, "Int16");
            registerProperty(TestComponent, m_i32, "Int32");
            registerProperty(TestComponent, m_i64, "Int64");
        }
        registerPropertyGroupEnd(TestComponent);

        registerPropertyGroupBegin(TestComponent, "Float");
        {
            registerProperty(TestComponent, m_float, "Float");
        }
        registerPropertyGroupEnd(TestComponent);

        registerPropertyGroupBegin(TestComponent, "Vector2");
        {
            registerProperty(TestComponent, m_float2, "Float2");
            registerProperty(TestComponent, m_uint2, "Uint2");
            registerProperty(TestComponent, m_int2, "Int2");      
        }
        registerPropertyGroupEnd(TestComponent);

        registerPropertyGroupBegin(TestComponent, "Vector3");
        {
            registerProperty(TestComponent, m_float3, "Float3");
            registerProperty(TestComponent, m_uint3, "Uint3");
            registerProperty(TestComponent, m_int3, "Int3");
        }
        registerPropertyGroupEnd(TestComponent);

        registerPropertyGroupBegin(TestComponent, "Vector4");
        {
            registerProperty(TestComponent, m_float4, "Float4");
            registerProperty(TestComponent, m_uint4, "Uint4");
            registerProperty(TestComponent, m_int4, "Int4");
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