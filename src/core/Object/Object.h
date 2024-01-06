#pragma once

#include "core/IObject.h"

#define VG_CLASS_SUPER_CLASSNAME(name, parent)		using super = parent;																\
													static const char * getStaticClassName  () { return #name; }						\
													const char * getClassName() const override { return name::getStaticClassName(); }	

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

        const IClassDesc *          getClassDesc        () const final;

								    Object		        (const string & _name, IObject * _parent = nullptr);
									Object				(const Object & _other);
								    Object		        ();
								    ~Object		        ();

		IObject *					Instanciate			() const override;

		void                        OnLoad				() override;

        void                        OnPlay				() override;
        void                        OnStop				() override;

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
		string					    m_name;

	private:
        string                      m_file;
		atomic<u32>				    m_refCount;
        Object *                    m_parent = nullptr;
	};
}