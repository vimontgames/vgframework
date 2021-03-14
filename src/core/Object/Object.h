#pragma once

#include "core/IObject.h"

namespace vg::core
{
    class IObjectFactory;

	class Object : public IObject
	{
	public:
								    Object		        (const string & _name);
								    Object		        ();
								    ~Object		        ();

        const IObjectDescriptor *   getClassDesc        () const final;
        static bool                 registerProperties  (IObjectDescriptor & _desc);

		void					    setName		        (const string & _name) override;
		const string &			    getName		        () const override;

		u32						    addRef		        () override;
		u32						    release		        () override;

		// Added so that we can use the same 'VG_SAFE_RELEASE' macro everywhere
		u32						    Release		        () final override;

	private:
		string					    m_name;
		atomic<u32>				    m_refCount;
	};
}