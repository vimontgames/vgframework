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
    class ResourceInfo final : public IResourceInfo
    {
    public:
        VG_CLASS_DECL(ResourceInfo, IResourceInfo);

        ResourceInfo(const string & _path);

        const core::string &                                GetPath             () const final override;
        const core::string                                  GetFolder           () const final override;
        const core::string                                  GetFilename         () const final override;
        const io::FileAccessTime                            GetLastModifiedDate () const final override;
        const io::FileAccessTime                            GetLastCookDate     () const final override;
        const core::u64                                     GetRawFileSize      () const final override;
        const core::u64                                     GetCookedFileSize   () const final override;
        const float                                         GetCookingDuration  () const final override;
        const float                                         GetLoadingDuration  () const final override;
        const core::string                                  GetExtension        () const final override;
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

        void                                                setCookingTime      (float _cookingTime);
        void                                                setLoadingTime      (float _loadingTime);
        void                                                setRawFileSize      (core::u64 _fileSize);                                
        void                                                setCookedFileSize   (core::u64 _fileSize);
        void                                                setLastModifiedDate (core::io::FileAccessTime _lastModified);
        void                                                setLastCookDate     (core::io::FileAccessTime _lastCooked);

    private:
        core::IObject *                                     m_object = nullptr;         // The shared object
        const core::string                                  m_path;                     // Path of the source data for the shared object
        core::vector<core::IResource *>                     m_clients;                  // Clients (Resource handles) using the shared object

        #if USE_CLIENTS_TO_UPDATE_LIST
        core::vector<core::IResource *>                     m_clientsToUpdate;
        #endif

        core::io::FileAccessTime                            m_lastModifiedDate  = (core::io::FileAccessTime)0;
        core::io::FileAccessTime                            m_lastCookDate      = (core::io::FileAccessTime)0;
        core::u64                                           m_rawFileSize       = 0;
        core::u64                                           m_cookedFileSize    = 0;
        float                                               m_cookingDuration   = 0.0f;         
        float                                               m_loadingDuration   = 0.0f;
        bool                                                m_forceReimport     = false;    // For reimport of resource on next load
    };
}

#if VG_ENABLE_INLINE
#include "ResourceInfo.inl"
#endif