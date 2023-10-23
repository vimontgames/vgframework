#pragma once

#include "renderer/Renderer_consts.h"

namespace vg::core
{
    class Logger;
}

namespace vg::engine
{
    struct EngineCreationParams
    {
        vg::renderer::RendererCreationParams renderer;
        vg::core::Logger * logger = nullptr;
    };
}