#pragma once

#include "application/IProject.h"
#include "core/Singleton/Singleton.h"

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
        const char *                getClassName            () const final { return "Game"; }

        // vg::core::IPlugin overrides
        vg::core::IPlugin::Version  getVersion              () const final;
        bool                        registerClasses         () final;
        bool                        unregisterClasses       ();

        // vg::core::IProject overrides
        bool                        init                    (vg::engine::IEngine & _engine, vg::core::Singletons & _singletons) final;
        bool                        deinit                  () final;

        bool                        update                  () final;

    protected:

    private:
        vg::engine::IEngine *       m_engine = nullptr;
};