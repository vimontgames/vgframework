#pragma once

#include "core/Resource/Resource.h"

namespace vg::engine
{
    class BaseSceneResource : public core::Resource
    {
    public:
        VG_CLASS_DECL(BaseSceneResource, core::Resource);

        BaseSceneResource(const core::string & _name = "", IObject * _parent = nullptr);
        ~BaseSceneResource();

        void                                OnResourcePathChanged   (const core::string & _oldPath, const core::string & _newPath) final;

        bool                                Cook                    (const core::string & _file) const override;

        bool                                CanCreateFile           () const final override { return true; };
        bool                                CanSaveFile             () const final override { return true; };

    protected:
        //void                                fixMissingUIDs          (core::IGameObject * _gameObject);
    };
}