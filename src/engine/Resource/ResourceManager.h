#pragma once

#include "core/Singleton/Singleton.h"
#include "engine/IResourceManager.h"
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

    enum class CookedStatus : core::u8
    {
        UP_TO_DATE = 0,             // File doesn't need cook

        RAWDATA_FILE_UPDATED,       // File needs cook because raw data file has been updated
        META_FILE_UPDATED,          // File needs cook because meta file has been updated
        NO_COOKED_FILE,             // File needs cook because no cooked file was found
        FORCE_REIMPORT,             // File needs cook because reimport is forced
        COOK_VERSION_DEPRECATED     // File needs cook because cooked file version is deprecated
    };

    class ResourceManager final : public IResourceManager, public core::Singleton<ResourceManager>
    {
    public:
        VG_CLASS_DECL(ResourceManager, IResourceManager);

        ResourceManager(const core::string & _name, IObject * _parent);
        ~ResourceManager();

        // Lock during editing
        void                        Lock                        () const final override;
        void                        Unlock                      () const final override;

        bool                        HasResourceLoading          () const final override;
        core::uint                  GetResourceCount            () const final override;
        const IResourceInfo &       GetResourceInfo             (core::uint _index) const final override;

        core::IResourceMeta *       GetOrCreateResourceMeta     (const core::IResource * _resource) const final override;
        core::IResourceMeta *       GetResourceMeta             (const core::string & _resourcePath) const final override;
        void                        SetResourceMeta             (const core::string & _resourcePath, core::IResourceMeta * _meta) final override;

        core::uint                  UpdateMeta                  () final override;
        core::uint                  UpdateResources             () final override;
        void                        Reimport                    (core::IResource * _res) final override;

        void                        loadResourceAsync           (core::Resource * _resource, const core::string & _oldPath, const core::string & _path);
        void                        unloadResource              (core::Resource * _resource, const core::string & _path);

        void                        updateLoading               ();
        void                        flushPendingLoading         ();

        bool                        isLoadingThreadRunning      () const { return m_isLoadingThreadRunning; }

    protected:
        static void                 loading                     (ResourceManager * _this);
        static CookedStatus         needsCook                   (const ResourceInfo & _info);

        void                        updateLoading               (bool _async);
        void                        loadOneResource             (ResourceInfo & _info);
        void                        updateMeta                  (const core::string & _folder, core::uint & _counter);

    private:
        std::thread                                             m_loadingThread;
        core::atomic<bool>                                      m_isLoadingThreadRunning = true;

        core::dictionary<ResourceInfo*>                         m_resourceInfosMap;
        core::dictionary<core::IResourceMeta *>                 m_resourceMeta;

        core::vector<core::IResource*>                          m_resourcesToLoad;
        core::vector<core::IResource*>                          m_resourcesLoaded;
        core::vector<core::IResource *>                         m_resourcesLoadedAsync;

        mutable core::recursive_mutex                           m_addResourceToLoadRecursiveMutex;
        mutable core::mutex                                     m_resourceLoadedAsyncMutex;
        
    };
}