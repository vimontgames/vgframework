#pragma once

#include "core/IObject.h"

#define VG_CLASS_PROPERTIES(name, parent)			using super = parent;																\
													static bool registerProperties(vg::core::IClassDesc & _desc);						\
													static bool registerClass(vg::core::IFactory & _factory);							\
													static const char * getStaticClassName  () { return #name; }						\
													const char * getClassName() const override { return name::getStaticClassName(); }	

#define VG_CLASS_CTOR_HEADER_IMPL(name, parent)		name(const core::string & _name, IObject * _parent) :								\
													parent(_name, _parent)																\
													{																					\
													}

#define VG_CLASS_REGISTER_PROP_IMPL(name, parent)	bool registerProperties(class vg::core::IClassDesc & _desc)							\
													{																					\
														return parent::registerProperties(_desc);										\
													}

#define VG_CLASS_DECL(name, parent)					VG_CLASS_PROPERTIES(name, parent)	

#define VG_CLASS_DECL_PASSTHROUGH(name, parent)		VG_CLASS_PROPERTIES(name, parent)													\
													VG_CLASS_CTOR_HEADER_IMPL(name, parent)												\
													//VG_CLASS_REGISTER_PROP_IMPL(name, parent)
												
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

        void                        setParent           (IObject * _parent) final;
        IObject *                   getParent           () const final;

        void                        onPropertyChanged   (IObject * _object, const IProperty & _prop) override;
        void                        onResourceLoaded    (IResource * _resource) override;
		void                        onResourceUnloaded	(IResource * _resource) override;

		void					    setName		        (const string & _name) override;
		const string &			    getName		        () const override;

        bool                        hasFile             () const override;
        void					    setFile             (const string & _file) override;
        const string &			    getFile             () const override;

		u32						    addRef		        () override;
		u32						    release		        () override;

		// Added so that we can use the same 'VG_SAFE_RELEASE' macro everywhere
		u32						    Release		        () final;

		static bool					resizeVector		(core::IObject * _parent, core::uint _offset, core::uint _count, void ** _data);

	private:
		string					    m_name;
        string                      m_file;
		atomic<u32>				    m_refCount;
        Object *                    m_parent = nullptr;
	};
}