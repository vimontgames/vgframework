#include "engine/Precomp.h"
#include "RenderStates.h"
#include "engine/Engine.h"
#include "renderer/IRendererOptions.h"

#if !VG_ENABLE_INLINE
#include "RenderStates.inl"
#endif

using namespace vg::core;
using namespace vg::renderer;

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS(StencilState, "StencilState");

    //--------------------------------------------------------------------------------------
    bool isStencilOptionReadOnly(const IObject * _object, const IProperty * _prop, uint _index)
    {
        const StencilState * state = VG_SAFE_STATIC_CAST(const StencilState, _object);
        return !state->isEnabled();
    }

    //--------------------------------------------------------------------------------------
    core::vector<core::string> getStencilRefNames(const IObject * _object, const IProperty * _prop, core::uint _index)
    {
        IRendererOptions * rendererOptions = Engine::get()->getRendererOptions();

        core::vector<core::string> names(enumCount<gfx::StencilBit>());
        for (uint i = 0; i < names.size(); ++i)
            names[i] = rendererOptions->GetStencilBitName((gfx::StencilBit)i);
        
        return names;
    }

    //--------------------------------------------------------------------------------------
    bool StencilState::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(StencilState, m_enable, "Enable");

        registerProperty(StencilState, m_ref, "Ref");
        setPropertyNamesCallback(StencilState, m_ref, getStencilRefNames);
        setPropertyReadOnlyCallback(StencilState, m_ref, isStencilOptionReadOnly);

        registerPropertyEnum(StencilState, gfx::ComparisonFunc, m_func, "Func");
        setPropertyReadOnlyCallback(StencilState, m_func, isStencilOptionReadOnly);

        registerPropertyEnum(StencilState, gfx::StencilOp, m_passOp, "Pass");
        setPropertyReadOnlyCallback(StencilState, m_passOp, isStencilOptionReadOnly);

        registerPropertyEnum(StencilState, gfx::StencilOp, m_failOp, "Fail");
        setPropertyReadOnlyCallback(StencilState, m_failOp, isStencilOptionReadOnly);

        registerPropertyEnum(StencilState, gfx::StencilOp, m_depthFailOp, "DepthFail");
        setPropertyReadOnlyCallback(StencilState, m_depthFailOp, isStencilOptionReadOnly);

        return true;
    }

    //--------------------------------------------------------------------------------------
    StencilState::StencilState(const vg::core::string & _name, vg::core::IObject * _parent) :
        super(_name, _parent),
        m_ref(8)
    {
    }

    //--------------------------------------------------------------------------------------
    StencilState::~StencilState()
    {
    }
}