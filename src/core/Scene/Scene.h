#pragma once

#include "core/IScene.h"

namespace vg::core
{
    class ISector;
    class IFactory;
    class IClassDesc;

    class Scene : public IScene
    {
    public:
        using super = IScene;

                        Scene               (const string & _name, IObject * _parent);
                        ~Scene              ();

        const char *    getClassName() const final { return "Scene"; }

        static bool     registerClass       (IFactory & _factory);
        static bool     registerProperties  (IClassDesc & _desc);

        static bool     load                (IObject * _object);
        static bool     save                (IObject * _object);

        void            setRoot             (ISector * _sector) final;
        ISector *       getRoot             () const final;

    private:
        ISector *       m_root              = nullptr;
    };
}