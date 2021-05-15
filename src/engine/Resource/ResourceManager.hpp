#include "ResourceManager.h"
#include "core/Resource/Resource.h"
#include "core/Object/AutoRegisterClass.h"
#include "core/Timer/Timer.h"

using namespace vg::core;

namespace vg::engine
{
    VG_AUTO_REGISTER_CLASS(ResourceManager);

    //--------------------------------------------------------------------------------------
    bool ResourceManager::registerClass(IObjectFactory & _factory)
    {
        if (IObjectDescriptor * desc = _factory.registerClassHelper(ResourceManager, "Resource Manager", IObjectDescriptor::Flags::None))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool ResourceManager::registerProperties(IObjectDescriptor & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerPropertyObjectPointerVectorHelper(ResourceManager, m_resources, "Resources", IPropertyDescriptor::Flags::ReadOnly);

        return true;
    }

    //--------------------------------------------------------------------------------------
    ResourceManager::ResourceManager(const string & _name, IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    ResourceManager::~ResourceManager()
    {

    }

    //--------------------------------------------------------------------------------------
    void ResourceManager::loadResourceAsync(Resource * _resource, const string & _path, IObject * _owner)
    {
        m_resourcesToLoad.emplace_back(_resource, _path, _owner);
    }

    //--------------------------------------------------------------------------------------
    void ResourceManager::flushLoading()
    {
        VG_PROFILE_CPU("FlushLoading");

        for (uint i = 0; i < m_resourcesLoaded.size(); ++i)
        {
            const auto & info = m_resourcesLoaded[i];
            info.m_owner->onResourceLoaded(info.m_resource);
        }
        m_resourcesLoaded.clear();

        auto resourcesToLoad = std::move(m_resourcesToLoad);

        for (uint i = 0; i < resourcesToLoad.size(); ++i)
        {
            auto & info = resourcesToLoad[i];

            auto it = m_resourcesMap.find(info.m_path);
            if (m_resourcesMap.end() == it)
            {
                const auto start = Timer::getTick();

                if (info.m_resource->loadResource(info.m_path, info.m_owner))
                {
                    m_resourcesMap.insert(make_pair(info.m_path, info.m_resource));
                    VG_DEBUGPRINT("[ResourceManager] Resource loaded in %.2f ms\n", Timer::getEnlapsedTime(start, Timer::getTick()));

                }
                else
                    VG_DEBUGPRINT("[ResourceManager] Error loading resource \"%s\"\n", info.m_path.c_str());
            }
            else
            {
                info.m_resource->setObject(it->second->getObject());
            }

            m_resourcesLoaded.push_back(info);
        }
    }
}