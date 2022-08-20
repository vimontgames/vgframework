#pragma once

#include "core/Types/Types.h"

namespace vg::core
{
    class IClassDesc;
    class IProperty;
    class IResource;

	class IObject
	{
	public:

        virtual const char *                getClassName        () const = 0;
        virtual const IClassDesc *          getClassDesc        () const = 0;

										    IObject		        () {}
		virtual							    ~IObject	        () {}

        virtual void                        setParent           (IObject * _parent) = 0;
        virtual IObject *                   getParent           () const = 0;

        virtual void                        onPropertyChanged   (const IProperty & _prop) = 0;
        virtual void                        onResourceLoaded    (IResource * _resource) = 0;

        virtual bool                        loadFromFile        (const string & _filename) = 0;
        virtual bool                        saveToFile          (const string & _filename) const = 0;

		virtual void					    setName		        (const string & _name) = 0;
		virtual const string &			    getName		        () const = 0;

		virtual u32						    addRef		        () = 0;
		virtual u32						    release		        () = 0;

		// Added alias so that we can use the same 'VG_SAFE_RELEASE' macro everywhere
		virtual u32						    Release		        () = 0;
	};
}