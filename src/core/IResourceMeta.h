#pragma once

#include "Object/Object.h"

namespace vg::core::io
{
    using FileAccessTime = u64;
}

namespace vg::core
{
    class IObject;

    //--------------------------------------------------------------------------------------
    // This class stores Resource's metadata (e.g., Texture resource import settings)
    //--------------------------------------------------------------------------------------
    class IResourceMeta : public Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IResourceMeta, Object);

        virtual bool                Load                (const core::string & _resourcePath) = 0;
        virtual bool                Save                (const core::string & _resourcePath) = 0;

        virtual io::FileAccessTime  GetLastWriteTime    () const = 0;
        virtual void                SetLastWriteTime    (io::FileAccessTime _lastWriteTime) = 0;
    };
}