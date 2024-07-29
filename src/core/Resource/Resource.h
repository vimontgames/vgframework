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

        void                        setup                   (IObject * _owner, const string & _path, UserData _userData = 0x0) final;

        bool                        setObject               (core::IObject * _object) final;
        IObject *                   getObject               () const final;
    
        void                        setUserData             (UserData _userData) final;
        UserData                    getUserData             () const final;

        bool                        ClearResourcePath       () final;
        bool                        SetResourcePath         (const string & _path) final;
        const string &              GetResourcePath         () const final;

        const vector<string>        getExtensions           () const override { vector<string> ext; return ext; };
        void                        onResourcePathChanged   (const string & _oldPath, const string & _newPath) override { };

        bool                        cook                    (const core::string & _file) const { return false;};

        core::IObject *             load                    (const core::string & _file) { return nullptr; };
        void                        unload                  (const core::string & _file);

        void                        loadSubResources        () override;
        void                        unloadSubResources      () override;

        bool                        CanCreateFile           () const override { return false; }
        bool                        CanSaveFile             () const override { return false; }

	protected:
        string                      m_resourcePath;
        IObject *                   m_object = nullptr;
        UserData                    m_userData = 0x0;
    };
}