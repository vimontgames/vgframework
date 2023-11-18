#pragma once

#include "Object/Object.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    // An IResource is an object that will from an external file with dependencies, e.g. a TextureResource will be used to reference a Texture in a MaterialResource used to reference a MaterialModel
    //--------------------------------------------------------------------------------------
    class IResource : public Object
    {
    public:
        using UserData = core::u64;

        IResource(const string & _name = "", IObject * _parent = nullptr) : Object(_name, _parent) { }
        IResource(const IResource & _other) : Object(_other) { }
        virtual ~IResource() = default;

        virtual bool                    CreateFile              (const string & _path) = 0;
        virtual bool                    SaveFile                (const string & _path) const = 0;

        virtual void                    setup                   (IObject * _owner, const string & _path, UserData _userData = 0x0) = 0;

        virtual bool                    setObject               (core::IObject * _object) = 0;
        virtual IObject *               getObject               () const = 0;

        virtual void                    setUserData             (UserData _userData) = 0;
        virtual UserData                getUserData             () const = 0;

        virtual bool                    Reimport                () = 0;

        virtual bool                    ClearResourcePath       () = 0;
        virtual bool                    SetResourcePath         (const string & _path) = 0;
        virtual const string &          GetResourcePath         () const = 0;
       
        virtual const vector<string>    getExtensions           () const = 0;

        virtual void                    onResourcePathChanged   (const string & _oldPath, const string & _newPath) = 0;

        virtual bool                    cook                    (const core::string & _file) const = 0;

        virtual core::IObject *         load                    (const core::string & _file) = 0;
        virtual void                    unload                  (const core::string & _file) = 0;

        virtual void                    loadSubResources        () = 0;
        virtual void                    unloadSubResources      () = 0;

        virtual bool                    CanCreateFile           () const = 0;
        virtual bool                    CanSaveFile             () const = 0;
    };
}