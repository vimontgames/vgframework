#pragma once

#include "Object/Object.h"

namespace vg::core
{
    class IObject;
    class IResourceMeta;

    //--------------------------------------------------------------------------------------
    // An IResource is an object that will from an external file with dependencies. 
    // e.g. a TextureResource used to reference a Texture in a MaterialResource used to reference a MaterialModel
    //--------------------------------------------------------------------------------------
    class IResource : public Object
    {
    public:
        using UserData = core::u64;

        IResource(const string & _name = "", IObject * _parent = nullptr) : Object(_name, _parent) { }
        IResource(const IResource & _other) : Object(_other) { }
        virtual ~IResource() = default;

        virtual bool                    CreateFile              (const string & _path, IObject * _data = nullptr) = 0;
        virtual bool                    SaveFile                (const string & _path) const = 0;

        virtual void                    Setup                   (IObject * _owner, const string & _path, UserData _userData = 0x0) = 0;

        virtual bool                    SetObject               (core::IObject * _object) = 0;
        virtual IObject *               GetObject               () const = 0;

        virtual void                    SetUserData             (UserData _userData) = 0;
        virtual UserData                GetUserData             () const = 0;

        virtual bool                    ClearResourcePath       () = 0;
        virtual bool                    SetResourcePath         (const string & _path) = 0;
        virtual const string &          GetResourcePath         () const = 0;
       
        virtual const vector<string>    GetExtensions           () const = 0;

        virtual void                    OnResourcePathChanged   (const string & _oldPath, const string & _newPath) = 0;

        virtual bool                    Cook                    (const core::string & _file) const = 0;

        virtual core::IObject *         Load                    (const core::string & _file) = 0;
        virtual void                    Unload                  (const core::string & _file) = 0;

        virtual void                    LoadSubResources        () = 0;
        virtual void                    UnloadSubResources      () = 0;

        virtual bool                    CanCreateFile           () const = 0;
        virtual bool                    CanSaveFile             () const = 0;

        virtual IResourceMeta *         CreateResourceMeta      (const core::string & _path) const = 0;
    };
}