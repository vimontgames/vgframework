#pragma once

#include "core/IResource.h"

namespace vg::core
{
    class IObject;
    class IObjectFactory;
    class IObjectDescriptor;

    class Resource : public IResource
    {
    public:
        const char *                getClassName        () const override   { return "Resource"; }   

        static bool                 registerClass       (IObjectFactory & _factory);
        static bool                 registerProperties  (IObjectDescriptor & _desc);

                                    Resource            (IObject * _owner);
                                    ~Resource           ();

        IObject *                   get                 () const final;      
        const string &              getPath             () const final;
        bool                        setPath             (const string & _path) final;

        bool                        loadResource        (const core::string & _path, core::IObject * _owner) = 0;
		bool						set					(core::IObject * _object) final;

	protected:
        string                      m_path;
        IObject *                   m_owner = nullptr;
        IObject *                   m_object = nullptr;
    };
}