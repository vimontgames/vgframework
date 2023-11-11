#pragma once

#include "core/Singleton/Singleton.h"
#include "engine/IResourceManager.h"

namespace vg::core
{
    class IFactory;
    class IClassDesc;
	class IModel;
    class Resource;
}

namespace vg::engine
{
    struct ResourceLoadInfo
    {
        ResourceLoadInfo(core::Resource * _resource, const core::string & _path, core::IObject * _owner) :
            m_resource(_resource),
            m_path(_path),
            m_owner(_owner)
        {

        }

        core::Resource *															m_resource;
        const core::string &														m_path;
        core::IObject *																m_owner;
    };

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

        void            loadResourceAsync       (core::Resource * _resource, const core::string & _path,  core::IObject * _owner);
        void            unloadResource          (core::Resource * _resource);

        void            updateLoading           ();
        void            flushPendingLoading     ();

        bool            isLoadingThreadRunning  () const { return m_isLoadingThreadRunning; }

    protected:
        static void     loading                 (ResourceManager * _this);
        static bool     needsCook               (const core::string & _resourcePath);

        void            updateLoading           (bool _async);
        void            loadOneResource         (ResourceLoadInfo & info);

    private:
        std::thread                             m_loadingThread;
        core::atomic<bool>                      m_isLoadingThreadRunning = true;

        core::vector<ResourceLoadInfo>          m_resourcesToLoad;
        core::vector<ResourceLoadInfo>          m_resourcesLoaded;
		core::dictionary<core::Resource*>       m_resourcesMap;

        core::recursive_mutex                   m_addResourceToLoadRecursiveMutex;
        core::mutex                             m_resourceLoadedAsyncMutex;
        core::vector<ResourceLoadInfo>          m_resourcesLoadedAsync;
    };
}