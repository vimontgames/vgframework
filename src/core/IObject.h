#pragma once

#include "core/Types/Types.h"

namespace vg::core
{
    class IClassDesc;
    class IProperty;
    class IResource;
    class IGameObject;

    enum class ObjectFlags : u32
    {
        NotSerialized   = 0x00000001,   // Object is not saved when its parent is serialized (e.g. for objects instantiated at runtime)
        DirtySave       = 0x00000002,   // Object has been modified and needs to be saved again or changes will be lost
        Opened          = 0x00000004,   // Object shows its children
        Prefab          = 0x00000008    // Object (e.g. World or GameObject) is a prefab
    };

    using UID = core::u32;

	class IObject
	{
	public:
        static const char *                 getStaticClassName  () { return "IObject"; }
        virtual const char *                GetClassName        () const = 0;

        virtual void                        SetClassDesc        (const IClassDesc * _classDesc) = 0;
        virtual const IClassDesc *          GetClassDesc        () const = 0;
        virtual bool                        IsRegisteredClass   () const= 0;

										    IObject		        () = default;
		virtual							    ~IObject	        () = default;

        virtual bool                        RegisterUID         () = 0;
        virtual bool                        HasValidUID         () const = 0;

        virtual UID                         GetUID              (bool _mustBeValid = true) const = 0;
        virtual void                        SetUID              (UID _uid) = 0;
        virtual bool                        CanCopyUID          () const = 0;

        virtual UID                         GetOriginalUID      (bool _mustBeValid = true) const = 0;
        virtual void						SetOriginalUID      (UID _uid) = 0;

        virtual ObjectFlags                 GetObjectFlags      () const = 0;
        virtual void                        SetObjectFlags      (ObjectFlags _flags, bool _enabled) = 0;

        virtual IObject *                   Instanciate         () const = 0;

        virtual void                        OnLoad              () = 0;

        virtual void                        OnPlay              () = 0;
        virtual void                        OnStop              () = 0;

        virtual void                        OnEnable            () = 0;
        virtual void                        OnDisable           () = 0;

        virtual void                        SetParent           (IObject * _parent) = 0;
        virtual IObject *                   GetParent           () const = 0;
        virtual IGameObject *               GetParentGameObject () const = 0;

        virtual void                        OnPropertyChanged   (IObject * _object, const IProperty & _prop, bool _notifyParent = true) = 0;
        virtual void						SetPropertyValue    (const IProperty & _prop, void * _previousValue, void * _newValue) = 0;

        virtual void                        onResourceLoaded    (IResource * _resource) = 0;
        virtual void                        onResourceUnloaded  (IResource * _resource) = 0;

		virtual void					    setName		        (const string & _name) = 0;
		virtual const string &			    getName		        () const = 0;
        virtual const string                GetFullName         () const = 0;
        virtual const string                GetShortName        () const = 0;

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