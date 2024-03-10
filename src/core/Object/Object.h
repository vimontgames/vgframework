#pragma once

#include "core/IObject.h"

#define VG_CLASS_SUPER_CLASSNAME(name, parent)		using super = parent;																\
													static const char * getStaticClassName  () { return #name; }						\
													const char * GetClassName() const override { return name::getStaticClassName(); }	

// Common class functions with default implementation for registerClass/registerProperty
#define VG_CLASS_PROPERTIES_IMPL(name, parent)		static bool registerProperties(vg::core::IClassDesc & _desc) { super::registerProperties(_desc); return true; }	

// Common class functions declaration only
#define VG_CLASS_PROPERTIES_DECL(name, parent)		static bool registerProperties(vg::core::IClassDesc & _desc);						\
													static bool registerClass(vg::core::IFactory & _factory);

// Default ctor
#define VG_CLASS_CTOR_HEADER_IMPL(name, parent)		name(const vg::core::string & _name = "", vg::core::IObject * _parent = nullptr) : parent(_name, _parent) { }

// Declare class that implements registerClass and registerProperties 
#define VG_CLASS_DECL(name, parent)					VG_CLASS_SUPER_CLASSNAME(name, parent)		\
													VG_CLASS_PROPERTIES_DECL(name, parent)	

// Declare class with default ctor but that implements registerClass and registerProperties 
#define VG_CLASS_DECL_PASSTHROUGH(name, parent)		VG_CLASS_SUPER_CLASSNAME(name, parent)		\
													VG_CLASS_PROPERTIES_DECL(name, parent)		\
													VG_CLASS_CTOR_HEADER_IMPL(name, parent)

// Declare class used for virtual interface
#define VG_CLASS_DECL_ABSTRACT(name, parent)		VG_CLASS_SUPER_CLASSNAME(name, parent)		\
													VG_CLASS_PROPERTIES_IMPL(name, parent)		\
													VG_CLASS_CTOR_HEADER_IMPL(name, parent)
												
namespace vg::core
{
    class IFactory;

	class Object : public IObject
	{
	public:
		VG_CLASS_DECL(Object, IObject);

		void                        SetClassDesc		(const IClassDesc * _classDesc) final override;
        const IClassDesc *          GetClassDesc        () const final override;

								    Object		        (const string & _name, IObject * _parent = nullptr);
									Object				(const Object & _other);
								    Object		        ();
								    ~Object		        ();

		ObjectFlags                 GetObjectFlags		() const final override;
		void                        SetObjectFlags		(ObjectFlags _flags, bool _enabled) final override;

		IObject *					Instanciate			() const override;

		void                        OnLoad				() override;

        void                        OnPlay				() override;
        void                        OnStop				() override;

		void                        OnEnable			() override;
		void                        OnDisable			() override;

        void                        setParent           (IObject * _parent) override;
        IObject *                   getParent           () const final override;

        void                        OnPropertyChanged   (IObject * _object, const IProperty & _prop, bool _notifyParent) override;

        void                        onResourceLoaded    (IResource * _resource) override;
		void                        onResourceUnloaded	(IResource * _resource) override;

		void					    setName		        (const string & _name) override;
		const string &			    getName		        () const override;

        bool                        hasFile             () const override;
        void					    setFile             (const string & _file) override;
        const string &			    getFile             () const override;

		u32						    addRef		        () override;
		u32						    release		        () override;
		u32                         getRefCount			() const override;

		// Added so that we can use the same 'VG_SAFE_RELEASE' macro everywhere
		u32						    Release		        () final;

		static bool					resizeVector		(core::IObject * _parent, core::uint _offset, core::uint _count, void ** _data);

	protected:
        VG_INLINE ObjectFlags       getObjectFlags		() const;
        VG_INLINE void              setObjectFlags		(ObjectFlags _flags, bool _enabled);

	private:
        atomic<u32>				    m_refCount;
		ObjectFlags					m_objectFlags = (ObjectFlags)0x0;
        Object *					m_parent = nullptr;
		mutable const IClassDesc *	m_classDesc = nullptr;
		string					    m_name;
        string                      m_file;
	};
}

#if VG_ENABLE_INLINE
#include "Object.inl"
#endif