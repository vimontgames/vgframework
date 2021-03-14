#pragma once

#include "application/IProject.h"

namespace vg::engine
{
    class IEngine;
}

class PlayerEntity;

class SuperVimontBrawl : public vg::IProject
{
    public:
                                    SuperVimontBrawl        ();
                                    ~SuperVimontBrawl       ();

        // vg::core::IObject overrides
        const char *                getClassName            () const final { return "SuperVimontBrawl"; }

        // vg::core::IPlugin overrides
        vg::core::IPlugin::Version  getVersion              () const final;
        bool                        registerClasses         () final;

        // vg::core::IProject overrides
        bool                        init                    (vg::engine::IEngine & _engine) final;
        bool                        deinit                  () final;

        bool                        update                  () final;

    protected:

    private:
        vg::engine::IEngine *       m_engine = nullptr;
        PlayerEntity *              m_player = nullptr;
};