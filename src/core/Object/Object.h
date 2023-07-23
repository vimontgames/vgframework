#pragma once

#include "core/IObject.h"

#define VG_CLASS_DECL(name, parent)	using super = parent;                                                               \
									static bool registerProperties(vg::core::IClassDesc & _desc);						\
									static bool registerClass(vg::core::IFactory & _factory);							\
									static const char * getStaticClassName  () { return #name; }                        \
									const char * getClassName() const override { return name::getStaticClassName(); }	


namespace vg::core
{
    class IFactory;

	class Object : public IObject
	{
	public:
		VG_CLASS_DECL(Object, IObject);

        const IClassDesc *          getClassDesc        () const final;

								    Object		        (const string & _name, IObject * _parent = nullptr);
								    Object		        ();
								    ~Object		        ();

        void                        setParent           (IObject * _parent) final;
        IObject *                   getParent           () const final;

        void                        onPropertyChanged   (const IProperty & _prop) override;
        void                        onResourceLoaded    (IResource * _resource) override;

		void					    setName		        (const string & _name) override;
		const string &			    getName		        () const override;

        bool                        hasFile             () const override;
        void					    setFile             (const string & _file) override;
        const string &			    getFile             () const override;

		u32						    addRef		        () override;
		u32						    release		        () override;

		// Added so that we can use the same 'VG_SAFE_RELEASE' macro everywhere
		u32						    Release		        () final;

	private:
		string					    m_name;
        string                      m_file;
		atomic<u32>				    m_refCount;
        Object *                    m_parent = nullptr;
	};
}