#pragma once

namespace vg
{
    namespace engine
    {
        class IEngine;
    }

    struct ApplicationCreationParams
    {
        engine::IEngine * engine = nullptr;
    };
}