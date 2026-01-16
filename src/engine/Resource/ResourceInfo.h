#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class Resource;
}

// When enabled, we keep track of the resource clients that need to be updated. 
// This way, we do not need to check if they need to be updated every frame and save ~0.2 ms (as of 14/01/26)
#define USE_CLIENTS_TO_UPDATE_LIST 1

namespace vg::engine
{
    class ResourceInfo : public IResourceInfo
    {
    public:
        VG_CLASS_DECL(ResourceInfo, IResourceInfo);

        ResourceInfo(const string & _path);

        const core::string &                                GetResourcePath     () const final override;
        const core::string                                  GetResourceType     () const final override;
        const core::uint                                    GetClientCount      () const final override;
        const core::IResource *                             GetClient           (core::uint _index) const final override;

        void                                                addClient           (core::IResource * _client);

        VG_INLINE const core::IObject *                     getObject           () const;
        VG_INLINE core::IObject *                           getObject           ();
        VG_INLINE core::IObject *&                          getObjectRef        ();

        VG_INLINE const core::string &                      getPath             () const;

        VG_INLINE const core::vector<core::IResource *> &   getClients          () const;

        #if USE_CLIENTS_TO_UPDATE_LIST
        VG_INLINE core::vector<core::IResource *> &         getClientsToUpdate  ();
        #endif

        bool                                                removeClient        (core::IResource * _clientToRemove);
        bool                                                replaceClient       (core::IResource * _clientToReplace, core::IResource * _newClient);

        VG_INLINE void                                      setForceReimport    (bool _forceReimport = true);
        VG_INLINE bool                                      needReimport        () const;

    private:
        core::IObject *                                     m_object = nullptr;         // The shared object
        const core::string                                  m_path;                     // Path of the source data for the shared object
        core::vector<core::IResource *>                     m_clients;                  // Clients (Resource handles) using the shared object

        #if USE_CLIENTS_TO_UPDATE_LIST
        core::vector<core::IResource *>                     m_clientsToUpdate;
        #endif

        bool                                                m_forceReimport = false;    // For reimport of resource on next load
    };
}

#if VG_ENABLE_INLINE
#include "ResourceInfo.inl"
#endif