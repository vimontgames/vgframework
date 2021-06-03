#pragma once

#include "core/Object/Object.h"
#include "core/Singleton/Singleton.h"

namespace vg::core
{
    class IObjectFactory;
    class IObjectDescriptor;
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

    class ResourceManager : public core::Object, public core::Singleton<ResourceManager>
    {
    public:
        using super = core::Object;

        const char *    getClassName() const final { return "ResourceManager"; }
        static bool     registerClass(core::IObjectFactory & _factory);
        static bool     registerProperties(core::IObjectDescriptor & _desc);

        ResourceManager(const core::string & _name, IObject * _parent);
        ~ResourceManager();

        void loadResourceAsync(core::Resource * _resource, const core::string & _path,  core::IObject * _owner);

        void updateLoading();
        void flushPendingLoading();

        bool isLoadingThreadRunning() const { return m_isLoadingThreadRunning; }

    protected:
        static void loading(ResourceManager * _this);

        void updateLoading(bool _async);
        void loadOneResource(ResourceLoadInfo & info);

    private:
        std::thread                                                                     m_loadingThread;
        core::atomic<bool>                                                              m_isLoadingThreadRunning = true;

        core::vector<core::Resource*>                                                   m_resources;

        core::vector<ResourceLoadInfo>													m_resourcesToLoad;
        core::vector<ResourceLoadInfo>													m_resourcesLoaded;
		core::unordered_map<core::string, core::Resource*, core::hash<core::string>>	m_resourcesMap;

        core::mutex                                                                     m_resourceLoadedAsyncMutex;
        core::vector<ResourceLoadInfo>													m_resourcesLoadedAsync;
    };
}