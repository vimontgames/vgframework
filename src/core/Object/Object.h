#pragma once

#include "core/IObject.h"

namespace vg::core
{
    class IObjectFactory;

	class ObjectPointer : public IObject
	{
	public:
        const IObjectDescriptor *   getClassDesc        () const final;
        static bool                 registerProperties  (IObjectDescriptor & _desc);

								    ObjectPointer		        (const string & _name, IObject * _parent);
								    ObjectPointer		        ();
								    ~ObjectPointer		        ();

        void                        setParent           (IObject * _parent) final;
        IObject *                   getParent           () const final;

        void                        onPropertyChanged   (const IPropertyDescriptor & _prop) override;
        void                        onResourceLoaded    (IResource * _resource) override;

        bool                        loadFromFile        (const string & _filename) override;
        bool                        saveToFile          (const string & _filename) override;

		void					    setName		        (const string & _name) override;
		const string &			    getName		        () const override;

		u32						    addRef		        () override;
		u32						    release		        () override;

		// Added so that we can use the same 'VG_SAFE_RELEASE' macro everywhere
		u32						    Release		        () final override;

	private:
		string					    m_name;
		atomic<u32>				    m_refCount;
        ObjectPointer *             m_parent = nullptr;
	};
}