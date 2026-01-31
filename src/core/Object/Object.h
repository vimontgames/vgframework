#pragma once

#include "core/IObject.h"
#include "core/Math/CRC.h"

namespace vg::core
{
    using ClassCRC = vg::core::u64;
}

#define VG_CLASS_SUPER_CLASSNAME(name, parent)		            using super = parent;																                    \
													            static const char * getStaticClassName      () { return #name; }						                \
													            const char * GetClassName                   () const override { return name::getStaticClassName(); }	\
                                                                static vg::core::ClassCRC getStaticClassCRC () { return crc64(#name); }                                 \
                                                                vg::core::ClassCRC GetClassCRC              () const override { return name::getStaticClassCRC(); }

// Common class functions with default implementation for registerClass/registerProperty
#define VG_CLASS_PROPERTIES_IMPL(name, parent)		            static bool registerProperties(vg::core::IClassDesc & _desc) { super::registerProperties(_desc); return true; }	 \
                                                                static bool registerClass(vg::core::IFactory & _factory);

// Common class functions declaration only
#define VG_CLASS_PROPERTIES_DECL(name, parent)		            static bool registerProperties(vg::core::IClassDesc & _desc);   \
													            static bool registerClass(vg::core::IFactory & _factory);

// Default ctor
#define VG_CLASS_CTOR_HEADER_IMPL(name, parent)		            name(const vg::core::string & _name = "", vg::core::IObject * _parent = nullptr) : parent(_name, _parent) { }

// Declare class that implements registerClass and registerProperties
// Use with 'VG_REGISTER_CLASS' in .cpp file
#define VG_CLASS_DECL(name, parent)					            VG_CLASS_SUPER_CLASSNAME(name, parent)		\
													            VG_CLASS_PROPERTIES_DECL(name, parent)	

// Declare class with default ctor but that implements registerClass and registerProperties 
// The class itself will no be visible for the factory, but its children can inherit from its properties
#define VG_CLASS_DECL_PASSTHROUGH(name, parent)		            VG_CLASS_SUPER_CLASSNAME(name, parent)		\
													            VG_CLASS_PROPERTIES_DECL(name, parent)		\
													            VG_CLASS_CTOR_HEADER_IMPL(name, parent)

// Declare class used for virtual interface
// Use with 'VG_REGISTER_ABSTRACT_CLASS' in .cpp file
// The factory will know about the class, but it can't be instantiated
#define VG_CLASS_DECL_ABSTRACT(name, parent)		            VG_CLASS_SUPER_CLASSNAME(name, parent)		\
													            VG_CLASS_PROPERTIES_IMPL(name, parent)		\
													            VG_CLASS_CTOR_HEADER_IMPL(name, parent)

// An abstract class that still declares properties
// Same as above but the "abstract" class declare properties
#define VG_CLASS_DECL_ABSTRACT_WITH_PROPERTIES(name, parent)    VG_CLASS_SUPER_CLASSNAME(name, parent)		\
													            VG_CLASS_PROPERTIES_DECL(name, parent)		\
													            VG_CLASS_CTOR_HEADER_IMPL(name, parent)
												
namespace vg::core
{
    class IFactory;

	class Object : public IObject
	{
	public:
		VG_CLASS_DECL(Object, IObject);

		void							SetClassDesc			(const IClassDesc * _classDesc) final override;
        const IClassDesc *				GetClassDesc			() const final override;
		bool							IsRegisteredClass		() const final override;

										Object					(const string & _name, IObject * _parent = nullptr);
										Object					(const Object & _other);
										Object					();
										~Object					();

        void                            Swap                    (IObject * _other) override;

		Object &						operator =				(const Object & _from);

		bool							RegisterUID				() override;
		bool							UnregisterUID			() override;
		bool							HasValidUID				() const final override;

		UID								GetUID					(bool _mustBeValid = true) const final override;
		void							SetUID					(UID _uid) override;
		bool							CanCopyUID				() const override { return false; }

       UID								GetOriginalUID			(bool _mustBeValid = true) const final override;
       void								SetOriginalUID			(UID _uid) final override;
                                
		ObjectFlags						GetObjectFlags			() const final override;
		void							SetObjectFlags			(ObjectFlags _flags, bool _enabled) final override;

        ObjectRuntimeFlags              GetObjectRuntimeFlags   () const final override;
        void                            SetObjectRuntimeFlags   (ObjectRuntimeFlags _flags, bool _enabled) final override;

		const IObject *					FindByOriginalUID		(UID _originalUID) const final override;

		IObject *						Instanciate				(InstanciateFlags _flags = (InstanciateFlags)0x0) const override;

		void							OnLoad					() override;
		void							OnSave					() override;

        void							OnPlay					() override;
        void							OnStop					() override;

		void							OnEnable				() override;
		void							OnDisable				() override;

        void							SetParent				(IObject * _parent) override;
        IObject *						GetParent				() const final override;
		IGameObject *					GetParentGameObject		() const final override;
		IComponent *					GetParentComponent		() const final override;

        void							OnPropertyChanged		(IObject * _object, const IProperty & _prop, bool _notifyParent) override;
		void							SetPropertyValue		(const IProperty & _prop, void * _previousValue, void * _newValue) override;
		
        void							OnResourceLoaded		(IResource * _resource) override;
		void							OnResourceUnloaded		(IResource * _resource) override;

		void							SetName					(const string & _name) override;
		const string &					GetName					() const override;
		const string 					GetFullName				() const override;
		const string					GetShortName			() const override;

        #if VG_OBJECT_HAS_STATIC_NAME
        const char *                    GetStaticName           () const override;
        #endif

        bool							HasFile					() const override;
        void							SetFile					(const string & _file) override;
        const string &					GetFile					() const override;

		u32								AddRef					() override;
		u32								Release					() override;
		u32								GetRefCount				() const override;

		bool							CanOverrideProperties	() const override;

        VG_INLINE void					setParent				(IObject * _parent);
		VG_INLINE IObject *				getParent				() const;

        VG_INLINE ObjectFlags			getObjectFlags			() const;
        VG_INLINE void					setObjectFlags			(ObjectFlags _flags, bool _enabled);

        VG_INLINE ObjectRuntimeFlags    getObjectRuntimeFlags	() const;
        VG_INLINE void                  setObjectRuntimeFlags	(ObjectRuntimeFlags _flags, bool _enabled);

		static bool						resizeVector			(core::IObject * _parent, core::uint _offset, core::uint _count, void ** _data);

	private:
        atomic<u32>                     m_refCount;
        UID                             m_uid                   = (UID)0;
        UID                             m_originalUID           = (UID)0;
        ObjectFlags                     m_objectFlags           = (ObjectFlags)0x0;
        ObjectRuntimeFlags              m_objectRuntimeFlags    = (ObjectRuntimeFlags)0x0;
        Object *                        m_parent                = nullptr;

		#ifndef VG_FINAL
		string                          m_parentNameDbg;
		#endif

        string                          m_name;
        string                          m_file;

        mutable const IClassDesc *      m_classDesc             = nullptr;

        #if VG_OBJECT_HAS_STATIC_NAME
        mutable std::atomic<const char *> m_staticName          = nullptr;
        #endif
	};
}

#if VG_ENABLE_INLINE
#include "Object.inl"
#endif