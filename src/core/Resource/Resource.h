#pragma once

#include "core/IResource.h"

namespace vg::core
{
    class IObject;
    class IFactory;
    class IClassDesc;

    class Resource : public IResource
    {
    public:
        VG_CLASS_DECL(Resource, IResource);

                                    Resource                (const core::string & _name = "", IObject * _parent = nullptr);
                                    Resource                (const Resource & _other);
                                    ~Resource               ();

        Resource &                  operator =              (const vg::core::Resource & _from);

        bool                        CreateFile              (const string & _path, IObject * _data) override;
        bool                        SaveFile                (const string & _path) const override;

        void                        Setup                   (IObject * _owner, const string & _path, UserData _userData = 0x0) final override;

        bool                        SetObject               (core::IObject * _object) final override;
        IObject *                   GetObject               () const final override;
    
        void                        SetUserData             (UserData _userData) final override;
        UserData                    GetUserData             () const final override;

        bool                        ClearResourcePath       () final override;
        bool                        SetResourcePath         (const string & _path) final override;
        const string &              GetResourcePath         () const final override;

        const vector<string>        GetExtensions           () const override;;
        void                        OnResourcePathChanged   (const string & _oldPath, const string & _newPath) override;

        bool                        Cook                    (const core::string & _file) const override;

        core::IObject *             Load                    (const core::string & _file) override;
        void                        Unload                  (const core::string & _file) override;

        void                        LoadSubResources        () override;
        void                        UnloadSubResources      () override;

        void                        OnResourceLoaded        (core::IResource * _resource) override;
        void                        OnResourceUnloaded      (core::IResource * _resource) override;

        bool                        CanCreateFile           () const override { return false; }
        bool                        CanSaveFile             () const override { return false; }

        IResourceMeta *             CreateResourceMeta      (const core::string & _path) const override;
        bool                        CanOverrideProperties   () const override;

	protected:
        string                      m_resourcePath;
        IObject *                   m_object = nullptr;
        UserData                    m_userData = 0x0;
    };
}