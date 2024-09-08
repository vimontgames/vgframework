#pragma once
#include "renderer/Instance/GraphicInstance.h"

namespace vg::renderer
{
    vg_enum_class(LightType, core::u8,
        Directional = 0,
        Omni,
        Spot
    );

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

        bool                            IsSkinned       () const final override { return false; }
        void                            Draw            (const RenderContext & _renderContext, gfx::CommandList * _cmdList) const override = 0;

        virtual LightType               GetLightType    () const = 0;
        virtual bool                    IsCastShadow    () const = 0;

        virtual void                    SetAmbient      (const core::float3 & _ambient) = 0;
        virtual const core::float3 &    GetAmbient      () const = 0;
    };
}