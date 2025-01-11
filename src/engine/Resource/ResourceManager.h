#pragma once

#include "core/Singleton/Singleton.h"
#include "core/IResourceManager.h"
#include "core/Object/Object.h"
#include "core/Scheduler/Mutex.h"

namespace vg::core
{
    class IFactory;
    class IClassDesc;
	class IModel;
    class Resource;
}

namespace vg::engine
{
    class ResourceInfo;

    vg_enum_class(CookStatus, core::u8,
        UP_TO_DATE = 0,             // File doesn't need cook

        RAWDATA_FILE_UPDATED,       // File needs cook because raw data file has been updated
        META_FILE_UPDATED,          // File needs cook because meta file has been updated
        NO_COOKED_FILE,             // File needs cook because no cooked file was found
        FORCE_REIMPORT,             // File needs cook because reimport is forced
        COOK_VERSION_DEPRECATED     // File needs cook because cooked file version is deprecated
    );

    class ResourceManager final : public core::IResourceManager, public core::Singleton<ResourceManager>
    {
    public:
        VG_CLASS_DECL(ResourceManager, core::IResourceManager);

        ResourceManager(const core::string & _name, IObject * _parent);
        ~ResourceManager();

        // Lock during editing
        void                        Lock                        () const final override;
        void                        Unlock                      () const final override;

        bool                        HasResourceLoading          () const final override;
        core::uint                  GetResourceCount            () const final override;
        const core::IResourceInfo & GetResourceInfo             (core::uint _index) const final override;

        core::IResourceMeta *       GetOrCreateResourceMeta     (const core::IResource * _resource) const final override;
        core::IResourceMeta *       GetResourceMeta             (const core::string & _resourcePath) const final override;
        void                        SetResourceMeta             (const core::string & _resourcePath, core::IResourceMeta * _meta) final override;

        void                        UpdateResources             (bool _async = true) final override;
        void                        Reimport                    (core::IResource * _res, bool _async) final override;

        void                        LoadResourceAsync           (core::IResource * _resource, const core::string & _oldPath, const core::string & _path) final override;
        void                        UnloadResource              (core::IResource * _resource, const core::string & _path) final override ;

        void                        updateLoadingMainThread     ();
        void                        flushPendingLoading         ();

        bool                        isLoadingThreadRunning      () const { return m_isLoadingThreadRunning; }
        void                        flushUpdateResource();

    protected:
        static void                 loading                     (ResourceManager * _this);
        static CookStatus           needsCook                   (const ResourceInfo & _info);

        void                        updateLoading               (bool _async);
        void                        loadOneResource             (ResourceInfo & _info);

        core::uint                  updateResources             ();
        void                        reimport                    (core::IResource * _res);

    private:
        std::thread                                             m_loadingThread;
        core::atomic<bool>                                      m_isLoadingThreadRunning = true;
        core::atomic<bool>                                      m_needUpdateResource = false;

        core::dictionary<ResourceInfo*>                         m_resourceInfosMap;
        core::dictionary<core::IResourceMeta *>                 m_resourceMeta;

        core::vector<core::IResource*>                          m_resourcesToLoad;
        core::vector<core::IResource*>                          m_resourcesLoaded;
        core::vector<core::IResource *>                         m_resourcesLoadedAsync;

        core::vector<core::IResource *>                         m_resourcesToReimport;

        mutable core::RecursiveMutex                            m_addResourceToLoadRecursiveMutex = core::RecursiveMutex("RecursiveMutex - AddResourceToLoad");
        mutable core::Mutex                                     m_resourceLoadedAsyncMutex = core::Mutex("Mutex - ResourceLoaded");
        
    };
}