#pragma once

#include "core/Singleton/Singleton.h"
#include "engine/IResourceManager.h"
#include "core/Object/Object.h"

namespace vg::core
{
    class IFactory;
    class IClassDesc;
	class IModel;
    class Resource;
}

namespace vg::engine
{
    class SharedResource;

    class ResourceManager : public IResourceManager, public core::Singleton<ResourceManager>
    {
    public:
        using super = IResourceManager;

        const char *    getClassName            () const final { return "ResourceManager"; }
        static bool     registerClass           (core::IFactory & _factory);
        static bool     registerProperties      (core::IClassDesc & _desc);

        ResourceManager(const core::string & _name, IObject * _parent);
        ~ResourceManager();

        bool            HasResourceLoading      () const final override;
        core::uint      GetResourceCount        () const final override;
        core::uint      UpdateResources         () final override;

        void            loadResourceAsync       (core::Resource * _resource, const core::string & _oldPath, const core::string & _path);
        void            unloadResource          (core::Resource * _resource, const core::string & _path);

        void            updateLoading           ();
        void            flushPendingLoading     ();

        bool            isLoadingThreadRunning  () const { return m_isLoadingThreadRunning; }

    protected:
        static void     loading                 (ResourceManager * _this);
        static bool     needsCook               (const core::string & _resourcePath);

        void            updateLoading           (bool _async);
        void            loadOneResource         (SharedResource & _shared);

    private:
        std::thread                             m_loadingThread;
        core::atomic<bool>                      m_isLoadingThreadRunning = true;

        core::dictionary<SharedResource*>       m_resourcesMap;

        core::vector<core::IResource*>          m_resourcesToLoad;
        core::vector<core::IResource*>          m_resourcesLoaded;
        core::vector<core::IResource *>         m_resourcesLoadedAsync;

        core::recursive_mutex                   m_addResourceToLoadRecursiveMutex;
        core::mutex                             m_resourceLoadedAsyncMutex;
        
    };
}