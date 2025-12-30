#pragma once

#include "core/IBaseScene.h"

namespace vg::renderer
{
    class IGraphicInstance;

    class IGraphicScene : public core::IBaseSceneRenderData
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IGraphicScene, core::IBaseSceneRenderData);

        virtual bool RegisterGraphicInstance(IGraphicInstance * _graphicInstance) = 0;
        virtual bool UnregisterGraphicInstance(IGraphicInstance * _graphicInstance) = 0;
    };
}
