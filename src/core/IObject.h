#pragma once

#include "core/Types/Types.h"

namespace vg::core
{
    class IClassDesc;
    class IProperty;
    class IResource;
    class IGameObject;
    class IComponent;

    // TODO: handle changing type for a smaller one and use u16 for 'serialized' flags and another u16 for 'runtime' flags instead? Or add serialization 'mask' for enum flag values?
    vg_enum_class(vg::core, ObjectFlags, u32,
        NotSerialized   = 0x00000001,   // Object is not saved when its parent is serialized (e.g., for objects instantiated at runtime)    (Runtime)
        DirtySave       = 0x00000002,   // Object has been modified and needs to be saved again or changes will be lost                     (Runtime)
        Opened          = 0x00000004,   // Object shows its children                                                                        (Serialized)
        Prefab          = 0x00000008,   // Object (e.g., World or GameObject) is a prefab                                                   (Runtime)
        Selected        = 0x00000010,   // Object is selected                                                                               (Serialized)
        NoCulling       = 0x00000020,   // Object is not influenced by any culling                                                          (Serialized)
        Unloaded        = 0x00000040    // Object is being unloaded                                                                         (Runtime)
    );

    // TODO: replace ObjectFlags
    vg_enum_class(vg::core, ObjectRuntimeFlags, u16,
        //NotSerialized   = 0x0001,   // Object is not saved when its parent is serialized (e.g., for objects instantiated at runtime)    (Runtime)
        //DirtySave       = 0x0002,   // Object has been modified and needs to be saved again or changes will be lost                     (Runtime)
        //Unloaded        = 0x0004,   // Object is being unloaded                                                                         (Runtime)
        //
        //Prefab          = 0x0010,   // Object (e.g., World or GameObject) is a prefab                                                   (Runtime)
        Instance        = 0x0020    // Object (e.g., World or GameObject) is a prefab                                                   (Runtime)
    );

    using UID = core::u32;

	class IObject
	{
	public:
        static const char *                 getStaticClassName      () { return "IObject"; }
        virtual const char *                GetClassName            () const = 0;

        virtual void                        SetClassDesc            (const IClassDesc * _classDesc) = 0;
        virtual const IClassDesc *          GetClassDesc            () const = 0;
        virtual bool                        IsRegisteredClass       () const= 0;

										    IObject		            () = default;
		virtual							    ~IObject	            () = default;

        virtual bool                        RegisterUID             () = 0;
        virtual bool                        UnregisterUID           () = 0;
        virtual bool                        HasValidUID             () const = 0;

        virtual UID                         GetUID                  (bool _mustBeValid = true) const = 0;
        virtual void                        SetUID                  (UID _uid) = 0;
        virtual bool                        CanCopyUID              () const = 0;

        virtual UID                         GetOriginalUID          (bool _mustBeValid = true) const = 0;
        virtual void						SetOriginalUID          (UID _uid) = 0;

        virtual ObjectFlags                 GetObjectFlags          () const = 0;
        virtual void                        SetObjectFlags          (ObjectFlags _flags, bool _enabled) = 0;

        virtual ObjectRuntimeFlags          GetObjectRuntimeFlags   () const = 0;
        virtual void                        SetObjectRuntimeFlags   (ObjectRuntimeFlags _flags, bool _enabled) = 0;

        virtual const IObject *             FindByOriginalUID       (UID _originalUID) const = 0;

        virtual IObject *                   Instanciate             () const = 0;

        virtual void                        OnLoad                  () = 0;
        virtual void                        OnSave                  () = 0;

        virtual void                        OnPlay                  () = 0;
        virtual void                        OnStop                  () = 0;

        virtual void                        OnEnable                () = 0;
        virtual void                        OnDisable               () = 0;

        virtual void                        SetParent               (IObject * _parent) = 0;
        virtual IObject *                   GetParent               () const = 0;
        virtual IGameObject *               GetParentGameObject     () const = 0;
        virtual IComponent *                GetParentComponent      () const = 0;

        virtual void                        OnPropertyChanged       (IObject * _object, const IProperty & _prop, bool _notifyParent = true) = 0;
        virtual void						SetPropertyValue        (const IProperty & _prop, void * _previousValue, void * _newValue) = 0;

        virtual void                        OnResourceLoaded        (IResource * _resource) = 0;
        virtual void                        OnResourceUnloaded      (IResource * _resource) = 0;

		virtual void					    SetName		            (const string & _name) = 0;
		virtual const string &			    GetName		            () const = 0;
        virtual const string                GetFullName             () const = 0;
        virtual const string                GetShortName            () const = 0;

        virtual bool                        HasFile                 () const = 0;
        virtual void					    SetFile                 (const string & _file) = 0;
        virtual const string &			    GetFile                 () const = 0;

		virtual u32						    AddRef		            () = 0;
		virtual u32						    Release		            () = 0;
        virtual u32                         GetRefCount             () const = 0;

        virtual bool                        CanOverrideProperties   () const = 0;
	};
}