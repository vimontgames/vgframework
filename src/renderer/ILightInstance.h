#pragma once
#include "renderer/Instance/GraphicInstance.h"

namespace vg::renderer
{
    vg_enum_class(vg::renderer, LightType, core::u8,
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
        VG_CLASS_DECL_ABSTRACT(ILightInstance, GraphicInstance);

        void                            Draw            (const RenderContext & _renderContext, gfx::CommandList * _cmdList) const override = 0;

        virtual LightType               GetLightType    () const = 0;
        virtual bool                    IsCastShadow    () const = 0;
    };
}