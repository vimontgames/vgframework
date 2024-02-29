#pragma once

#include "core/Types/Types.h"

namespace vg::core
{
    class IClassDesc;
    class IProperty;
    class IResource;

    enum class ObjectFlags : u32
    {
        NotSerialized   = 0x00000001,   // Object is not saved when its parent is serialized (e.g. for objects instantiated at runtime)
        DirtySave       = 0x00000002,   // Object has been modified and needs to be saved again or changes will be lost
        Opened          = 0x00000004,   // Object shows its children
        Prefab          = 0x00000008    // Object (e.g. World or GameObject) is a prefab
    };

	class IObject
	{
	public:
        static const char *                 getStaticClassName  () { return "IObject"; }
        virtual const char *                getClassName        () const = 0;

        virtual const IClassDesc *          getClassDesc        () const = 0;

										    IObject		        () = default;
		virtual							    ~IObject	        () = default;

        virtual ObjectFlags                 GetObjectFlags      () const = 0;
        virtual void                        SetObjectFlags      (ObjectFlags _flags, bool _enabled) = 0;

        virtual IObject *                   Instanciate         () const = 0;

        virtual void                        OnLoad              () = 0;

        virtual void                        OnPlay              () = 0;
        virtual void                        OnStop              () = 0;

        virtual void                        setParent           (IObject * _parent) = 0;
        virtual IObject *                   getParent           () const = 0;

        virtual void                        OnPropertyChanged   (IObject * _object, const IProperty & _prop, bool _notifyParent = true) = 0;

        virtual void                        onResourceLoaded    (IResource * _resource) = 0;
        virtual void                        onResourceUnloaded  (IResource * _resource) = 0;

		virtual void					    setName		        (const string & _name) = 0;
		virtual const string &			    getName		        () const = 0;

        virtual bool                        hasFile             () const = 0;
        virtual void					    setFile             (const string & _file) = 0;
        virtual const string &			    getFile             () const = 0;

		virtual u32						    addRef		        () = 0;
		virtual u32						    release		        () = 0;
        virtual u32                         getRefCount         () const = 0;

		// Added alias so that we can use the same 'VG_SAFE_RELEASE' macro everywhere
		virtual u32						    Release		        () = 0;
	};
}