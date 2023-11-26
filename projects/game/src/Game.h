#pragma once

#include "application/IProject.h"
#include "core/Singleton/Singleton.h"

namespace vg::core
{
    class IInput;
}

namespace vg::engine
{
    class IEngine;
}

class Game : public vg::IProject, public vg::core::Singleton<Game>
{
    public:
                                    Game        ();
                                    ~Game       ();

        // vg::core::IObject overrides
        const char *                    getClassName            () const final { return "Game"; }

        // vg::core::IPlugin overrides
        vg::core::IPlugin::Version      GetVersion              () const final override;
        bool                            RegisterClasses         () final override;
        bool                            UnregisterClasses       ();

        // vg::core::IProject overrides
        bool                            init                    (vg::engine::IEngine & _engine, vg::core::Singletons & _singletons) final;
        bool                            deinit                  () final;

        bool                            update                  () final;

        static vg::engine::IEngine &    Engine                  ();
        static vg::core::IInput &       Input                   ();

    protected:

    private:
        static vg::engine::IEngine *    s_engine;
};