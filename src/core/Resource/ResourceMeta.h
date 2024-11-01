#pragma once

#include "core/IResourceMeta.h"

namespace vg::core
{
    class ResourceMeta : public IResourceMeta
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(ResourceMeta, IResourceMeta);

        bool                Load                (const core::string & _resourcePath) final override;
        bool                Save                (const core::string & _resourcePath) final override;

        void                OnPropertyChanged   (IObject * _object, const IProperty & _prop, bool _notifyParent);

        io::FileAccessTime  GetLastWriteTime    () const final override;
        void                SetLastWriteTime    (io::FileAccessTime _lastWriteTime) final override;

    private:
        io::FileAccessTime  m_lastWriteTime = 0;
    };
}