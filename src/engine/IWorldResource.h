#pragma once

#include "core/Resource/Resource.h"

namespace vg::core
{
    class IWorld;
    class IScene;
    class IResource;
}

namespace vg::engine
{
    class IWorldResource : public core::Resource
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IWorldResource, core::Resource);

        virtual core::IWorld * GetWorld() const = 0;

        virtual void CreateSceneResource(const core::string & _file) = 0;
        virtual void LoadSceneResource(const core::string & _file) = 0;
        virtual IResource * FindSceneResource(core::IScene * _scene) = 0;
        virtual void UnloadSceneResource(core::IResource * _resource) = 0;

        virtual core::uint GetSceneResourceCount() const = 0;
        virtual core::IResource * GetSceneResource(core::uint _index) const = 0;
    };
}