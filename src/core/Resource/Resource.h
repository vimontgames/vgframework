#pragma once

#include "core/IResource.h"

namespace vg::core
{
    class Object;
    class IObjectFactory;
    class IObjectDescriptor;

    class Resource : public IResource
    {
    public:
        const char *                getClassName        () const override   { return "Resource"; }   

        IObject *                   get                 () const final;      
        const string &              getPath             () const final;
        bool                        setPath             (const string & _path) final;
        void                        onPathChanged       (const string & _oldPath, const string & _newPath);

        static bool                 registerClass       (IObjectFactory & _factory);
        static bool                 registerProperties  (IObjectDescriptor & _desc);

    private:
        string      m_path;
        Object *    m_object = nullptr;
    };
}