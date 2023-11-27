#pragma once

#include "core/IUniverse.h"

namespace vg::core
{
    class Scene;
    class IFactory;
    class IClassDesc;

    class Universe : public IUniverse
    {
    public:
        VG_CLASS_DECL(Universe, IUniverse);

        Universe(const string & _name, IObject * _parent);
        ~Universe();

        bool                    SetActiveScene(IScene * _scene) final override;
        core::IScene *          GetActiveScene() const final override;

        bool                    AddScene(IScene * _scene) final override;
        bool                    RemoveScene(core::IScene * scene) final override;
        core::uint              RemoveAllScenes() final override;

        uint                    GetSceneCount() const final override;
        const IScene *          GetScene(uint _index) const final override;

        vector<Scene*> &        getScenes();

    private:
        Scene *                 m_activeScene = nullptr;
        vector<Scene*>          m_scenes;
    };
}