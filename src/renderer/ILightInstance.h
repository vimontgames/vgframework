#pragma once
#include "renderer/Instance/GraphicInstance.h"

namespace vg::renderer
{
    enum class LightType : core::u8
    {
        Directional = 0,
        Omni,
        Spot
    };

    enum class ShadowType : core::u8
    {
        None,
        ShadowMap,
        Hybrid,
        RayTraced
    };

    class ILightDesc : public core::Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(ILightDesc, core::Object)
        virtual LightType GetLightType() const = 0;
    };

    class ILightInstance : public GraphicInstance
    {
    public:
        ILightInstance(const core::string & _name, core::IObject * _parent) :
            GraphicInstance(_name, _parent)
        {
            
        }

        bool                IsSkinned   () const final override { return false; }
        void                Draw        (const RenderContext & _renderContext, gfx::CommandList * _cmdList) const override = 0;

        virtual LightType   GetLightType() const = 0;
        virtual ShadowType  GetShadowType() const = 0;
    };
}