#pragma once

#include "core/IScene.h"

namespace vg::core
{
    class ISector;
    class IObjectFactory;
    class IObjectDescriptor;

    class Scene : public IScene
    {
    public:
        using super = IScene;

                        Scene               (const string & _name, IObject * _parent);
                        ~Scene              ();

        const char *    getClassName() const final { return "Scene"; }

        static bool     registerClass       (IObjectFactory & _factory);
        static bool     registerProperties  (IObjectDescriptor & _desc);

        static bool     load                (IObject * _object);
        static bool     save                (IObject * _object);

        void            setRoot             (ISector * _sector) final;
        ISector *       getRoot             () const final;

    private:
        ISector *       m_root              = nullptr;
    };
}