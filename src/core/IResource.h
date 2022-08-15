#pragma once

#include "Object/Object.h"

namespace vg::core
{
    class IResource : public Object
    {
    public:
        using UserData = core::u64;

        virtual ~IResource() {}

        virtual void                setup               (IObject * _owner, const string & _path, UserData _userData = 0x0) = 0;

        virtual bool                setObject           (core::IObject * _object) = 0;
        virtual IObject *           getObject           () const = 0;

        virtual void                setUserData         (UserData _userData) = 0;
        virtual UserData            getUserData         () const = 0;

        virtual void                setOwner            (core::IObject * _object) = 0;
        virtual core::IObject *     getOwner            () const = 0;

        virtual bool                setPath             (const string & _path) = 0;
        virtual const string &      getPath             () const = 0;
       
        virtual const vector<string> getExtensions      () const = 0;

        virtual void                onPathChanged       (IObject * _owner, const string & _oldPath, const string & _newPath) = 0;

        virtual bool                cook                (const core::string & _file) = 0;
        virtual bool                load                (const core::string & _file, IObject * _owner) = 0;

        virtual uint                getSubResourceCount () const = 0;
        virtual IResource *         getSubResource      (uint _index) = 0;
    };
}