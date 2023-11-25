#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class Resource;
}

namespace vg::engine
{
    class SharedResource : public IResourceInfo
    {
    public:
        VG_CLASS_DECL(SharedResource, IResourceInfo);

        SharedResource(const string & _name, IObject * _parent = nullptr);

        const core::string &    GetResourcePath () const final override;
        const core::string      GetResourceType () const final override;
        const core::uint        GetClientCount  () const final override;
        const core::IResource * GetClient       (core::uint _index) const final override;

        core::IObject *                     m_object = nullptr;         // The shared object
        core::string                        m_path;                     // Path of the source data for the shared object
        core::vector<core::IResource *>     m_clients;                  // Clients (Resource handles) using the shared object
        bool                                m_forceReimport = false;    // For reimport of resource on next load
    };
}