#pragma once

#include "core/IResource.h"

namespace vg::core
{
    class IObject;
    class IObjectFactory;
    class IObjectDescriptor;

    class Resource : public IResource
    {
    public:
        const char *                getClassName        () const override   { return "Resource"; }   

        static bool                 registerClass       (IObjectFactory & _factory);
        static bool                 registerProperties  (IObjectDescriptor & _desc);

                                    Resource            (IObject * _owner);
                                    Resource            (const Resource & _other);
                                    ~Resource           ();

        void                        setup               (IObject * _owner, const string & _path, UserData _userData = 0x0) final;

        bool                        setObject           (core::IObject * _object) final;
        IObject *                   getObject           () const final;
    
        void                        setUserData         (UserData _userData) final;
        UserData                    getUserData         () const final;

        void                        setOwner            (core::IObject * _object) final;
        core::IObject *             getOwner            () const final;

        bool                        setPath             (const string & _path) final;
        const string &              getPath             () const final;

        bool                        cook                (const core::string & _file) = 0;
        bool                        load                (const core::string & _file, core::IObject * _owner) = 0;

	protected:
        string                      m_path;
        IObject *                   m_owner = nullptr;
        IObject *                   m_object = nullptr;
        UserData                    m_userData = 0x0;
    };
}