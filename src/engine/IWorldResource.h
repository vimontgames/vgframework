#pragma once

#include "core/Resource/Resource.h"
#include "core/IBaseScene.h"

namespace vg::core
{
    class IWorld;
    class IBaseScene;
    class IResource;
}

namespace vg::engine
{
    class IWorldResource : public core::Resource
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IWorldResource, core::Resource);

        virtual core::IWorld * GetWorld() const = 0;

        virtual void CreateSceneResource(const core::string & _file, core::BaseSceneType _sceneType) = 0;
        virtual void LoadSceneResource(const core::string & _file, core::BaseSceneType _sceneType) = 0;
        virtual IResource * FindSceneResource(core::IBaseScene * _scene, core::BaseSceneType _sceneType) = 0;
        virtual void UnloadSceneResource(core::IResource * _resource, core::BaseSceneType _sceneType) = 0;

        virtual core::uint GetSceneResourceCount(core::BaseSceneType _sceneType) const = 0;
        virtual core::IResource * GetSceneResource(core::uint _index, core::BaseSceneType _sceneType) const = 0;
    };
}