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



    private:
        string                      m_path;
        IObject *                   m_owner = nullptr;
        IObject *                   m_object = nullptr;
    };
}