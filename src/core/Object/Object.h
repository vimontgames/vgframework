#pragma once

#include "core/IObject.h"

namespace vg::core
{
    class IFactory;

	class Object : public IObject
	{
	public:
        const IClassDesc *          getClassDesc        () const final;
        static bool                 registerProperties  (IClassDesc & _desc);

								    Object		        (const string & _name, IObject * _parent);
								    Object		        ();
								    ~Object		        ();

        void                        setParent           (IObject * _parent) final;
        IObject *                   getParent           () const final;

        void                        onPropertyChanged   (const IProperty & _prop) override;
        void                        onResourceLoaded    (IResource * _resource) override;

		void					    setName		        (const string & _name) override;
		const string &			    getName		        () const override;

		u32						    addRef		        () override;
		u32						    release		        () override;

		// Added so that we can use the same 'VG_SAFE_RELEASE' macro everywhere
		u32						    Release		        () final;

	private:
		string					    m_name;
		atomic<u32>				    m_refCount;
        Object *                    m_parent = nullptr;
	};
}