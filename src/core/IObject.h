#pragma once

#include "core/Types/Types.h"

namespace vg::core
{
    class IObjectDescriptor;
    class IPropertyDescriptor;

	class IObject
	{
	public:
										    IObject		        () {}
		virtual							    ~IObject	        () {}

        virtual const char *                getClassName        () const = 0;
        virtual const IObjectDescriptor *   getClassDesc        () const = 0;

        virtual void                        onPropertyChanged   (const IPropertyDescriptor & _prop) = 0;

        virtual bool                        loadFromFile        (const string & _filename) = 0;
        virtual bool                        saveToFile          (const string & _filename) = 0;

		virtual void					    setName		        (const string & _name) = 0;
		virtual const string &			    getName		        () const = 0;

		virtual u32						    addRef		        () = 0;
		virtual u32						    release		        () = 0;

		// Added alias so that we can use the same 'VG_SAFE_RELEASE' macro everywhere
		virtual u32						    Release		        () = 0;
	};
}