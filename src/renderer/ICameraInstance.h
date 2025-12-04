#pragma once
#include "renderer/Instance/GraphicInstance.h"

namespace vg::renderer
{
    class ICameraInstance: public GraphicInstance
    {
    public:
        VG_CLASS_DECL_ABSTRACT(ICameraInstance, GraphicInstance);

        bool    Cull        (const ViewCullingOptions & _cullingOptions, CullingResult * _cullingResult) override = 0;
        void    Draw        (const RenderContext & _renderContext, gfx::CommandList * _cmdList) const override = 0;
    };
}