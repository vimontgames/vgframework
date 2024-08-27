#pragma once

#include "engine/IWorldResource.h"
#include "engine/Resource/BaseScene/Scene/SceneResource.h"

namespace vg::engine
{
    class WorldResource : public IWorldResource
    {
    public:
        VG_CLASS_DECL(WorldResource, IWorldResource);

        WorldResource(const core::string & _name = "", IObject * _parent = nullptr);
        ~WorldResource();

        core::IWorld *                      GetWorld                () const final override;

        void                                CreateSceneResource     (const core::string & _file, core::BaseSceneType _sceneType) final override;
        void                                LoadSceneResource       (const core::string & _file, core::BaseSceneType _sceneType) final override;
        IResource *                         FindSceneResource       (core::IBaseScene * _scene, core::BaseSceneType _sceneType) final override;
        //void                                UnloadSceneResource     (core::IResource * _resource, core::BaseSceneType _sceneType) final override;
        void                                RemoveSceneResource     (core::IResource * _resource, core::BaseSceneType _sceneTyp) final override;

        core::uint                          GetSceneResourceCount   (core::BaseSceneType _sceneType) const final override;
        core::IResource *                   GetSceneResource        (core::uint _index, core::BaseSceneType _sceneType) const final override;

        const core::vector<core::string>    GetExtensions           () const final;
        void                                OnResourcePathChanged   (const core::string & _oldPath, const core::string & _newPath) final override;

        void                                onResourceLoaded        (core::IResource * _resource) final override;
        void                                onResourceUnloaded      (core::IResource * _resource) final override;

        bool                                Cook                    (const core::string & _file) const final override;
        core::IObject *                     Load                    (const core::string & _path) final override;

        bool                                CreateFile              (const core::string & _path, core::IObject * _data = nullptr) final override;
        bool                                SaveFile                (const core::string & _path) const override;

        bool                                CanCreateFile           () const final override { return true; };
        bool                                CanSaveFile             () const final override { return true; };

    private:
        static void * ResizeWorldResourceVector(core::IObject * _parent, core::uint _offset, core::uint _count, core::uint & _elementSize);
    };
}