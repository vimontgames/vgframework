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
        using super = IUniverse;

        Universe(const string & _name, IObject * _parent);
        ~Universe();

        const char *            getClassName() const final { return "Universe"; }

        static bool             registerClass(IFactory & _factory);
        static bool             registerProperties(IClassDesc & _desc);

        virtual bool            setActiveScene(IScene * _scene) final;
        virtual core::IScene *  getActiveScene() const final;

        virtual bool            addScene(IScene * _scene) final;
        virtual bool            removeScene(core::IScene * scene) final;
        virtual core::uint      removeAllScenes() final;

        virtual uint            getSceneCount() const final;
        virtual const IScene *  getScene(uint _index) const final;

    private:
        Scene *                 m_activeScene = nullptr;
        vector<Scene*>          m_scenes;
    };
}