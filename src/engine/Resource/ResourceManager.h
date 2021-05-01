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

        void flushLoading();

    private:
        core::vector<core::Resource*>   m_resources;

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
        core::vector<ResourceLoadInfo>													m_resourcesToLoad;
        core::vector<ResourceLoadInfo>													m_resourcesLoaded;
		core::unordered_map<core::string, core::Resource*, core::hash<core::string>>	m_resourcesMap;
    };
}