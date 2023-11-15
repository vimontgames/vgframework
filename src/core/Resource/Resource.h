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
        const char *                getClassName        () const override   { return "Resource"; }   

        static bool                 registerClass       (IFactory & _factory);
        static bool                 registerProperties  (IClassDesc & _desc);

                                    Resource            (const core::string & _name, IObject * _parent);
                                    Resource            (const Resource & _other);
                                    ~Resource           ();

        bool                        CreateFile          (const string & _path) override;
        bool                        SaveFile            (const string & _path) const override;

        void                        setup               (IObject * _owner, const string & _path, UserData _userData = 0x0) final;

        bool                        setObject           (core::IObject * _object) final;
        IObject *                   getObject           () const final;
    
        void                        setUserData         (UserData _userData) final;
        UserData                    getUserData         () const final;

        bool                        Reimport            () final;

        bool                        ClearResourcePath   () final;
        bool                        SetResourcePath     (const string & _path) final;
        const string &              GetResourcePath     () const final;

        bool                        cook                (const core::string & _file) const = 0;

        core::IObject *             load                (const core::string & _file) = 0;
        void                        unload              (const core::string & _file);

        void                        loadSubResources    () override;
        void                        unloadSubResources  () override;

	protected:
        string                      m_resourcePath;
        IObject *                   m_object = nullptr;
        UserData                    m_userData = 0x0;
    };
}