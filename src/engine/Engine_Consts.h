#pragma once

#include "renderer/Renderer_consts.h"
#include "physics/Physics_Consts.h"
#include "audio/Audio_Consts.h"

namespace vg::core
{
    class Logger;
}

namespace vg::engine
{
    struct EngineCreationParams
    {
        vg::renderer::RendererCreationParams renderer;
        vg::physics::PhysicsCreationParams physics;
        vg::audio::AudioCreationParams audio;
        vg::core::Logger * logger = nullptr;
    };
}