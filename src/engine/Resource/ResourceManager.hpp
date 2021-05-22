#include "ResourceManager.h"
#include "core/Resource/Resource.h"
#include "core/Object/AutoRegisterClass.h"
#include "core/Timer/Timer.h"
#include "core/File/File.h"

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

        static u32 skipFrames = 2;
        if (skipFrames != 0)
        {
            --skipFrames;
            return;
        }

        for (uint i = 0; i < m_resourcesLoaded.size(); ++i)
        {
            const auto & info = m_resourcesLoaded[i];
            info.m_owner->onResourceLoaded(info.m_resource);
        }
        m_resourcesLoaded.clear();

        while (m_resourcesToLoad.size() > 0)
        {
            auto resourcesToLoad = std::move(m_resourcesToLoad);

            for (uint i = 0; i < resourcesToLoad.size(); ++i)
            {
                auto & info = resourcesToLoad[i];

                auto it = m_resourcesMap.find(info.m_path);
                if (m_resourcesMap.end() == it)
                {
                    // check for an up-to-date cooked version of the resource
                    const string cooked = io::getCookedPath(info.m_path);
                    bool needCook = false;
                    bool done = false;
                    while (!done)
                    {
                        io::FileAccessTime rawDataLastWrite;
                        if (io::getLastWriteTime(info.m_path, &rawDataLastWrite))
                        {
                            if (io::exists(cooked))
                            {
                                io::FileAccessTime cookedFilelastWrite;
                                if (io::getLastWriteTime(cooked, &cookedFilelastWrite))
                                {
                                    if (cookedFilelastWrite != rawDataLastWrite)
                                        needCook = true;
                                }
                            }
                            else
                                needCook = true;
                        }

                        const auto startCook = Timer::getTick();
                        if (needCook)
                        {
                            VG_DEBUGPRINT("[ResourceManager] File \"%s\" is outdated.\n", info.m_path.c_str());

                            if (info.m_resource->cook(info.m_path))
                            {
                                if (io::setLastWriteTime(cooked, rawDataLastWrite))
                                    VG_DEBUGPRINT("[ResourceManager] Cooked \"%s\" successfully ... %.2f ms\n", info.m_path.c_str(), Timer::getEnlapsedTime(startCook, Timer::getTick()));
                            }
                        }

                        const auto startLoad = Timer::getTick();
                        if (info.m_resource->load(info.m_path, info.m_owner))
                        {
                            m_resourcesMap.insert(make_pair(info.m_path, info.m_resource));
                            VG_DEBUGPRINT("[ResourceManager] Resource \"%s\" loaded ... %.2f ms\n", info.m_path.c_str(), Timer::getEnlapsedTime(startLoad, Timer::getTick()));
                            done = true;
                        }
                        else
                        {
                            if (!needCook)
                            {
                                needCook = true;
                            }
                            else
                            {
                                VG_DEBUGPRINT("[ResourceManager] Could not load resource \"%s\"\n", info.m_path.c_str());
                                done = true;
                            }
                        }
                    }
                }
                else
                {
                    info.m_resource->setObject(it->second->getObject());
                }

                m_resourcesLoaded.push_back(info);
            }
        }
    }
}