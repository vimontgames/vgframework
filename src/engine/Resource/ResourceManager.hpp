#include "ResourceManager.h"
#include "core/Resource/Resource.h"
#include "core/Object/AutoRegisterClass.h"
#include "core/Timer/Timer.h"
#include "core/File/File.h"
#include "core/Scheduler/Scheduler.h"

using namespace vg::core;

#define VG_RESOURCE_MANAGER_ASYNC_LOADING 1

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
        super(_name, _parent),
        m_loadingThread(loading, this)
    {

    }

    //--------------------------------------------------------------------------------------
    ResourceManager::~ResourceManager()
    {
        flushPendingLoading();
    }

    //--------------------------------------------------------------------------------------
    void ResourceManager::flushPendingLoading()
    {
        if (true == m_isLoadingThreadRunning.exchange(false))
            m_loadingThread.join();
    }

    //--------------------------------------------------------------------------------------
    void ResourceManager::loading(ResourceManager * _this)
    {
        #if VG_RESOURCE_MANAGER_ASYNC_LOADING

        Kernel::getProfiler()->registerThread("Loading");

        while (_this->isLoadingThreadRunning())
        {
            _this->updateLoading(true);
            Sleep(0);
        }
        #endif
    }

    //--------------------------------------------------------------------------------------
    void ResourceManager::updateLoading(bool _async)
    {
        auto resourcesToLoad = std::move(m_resourcesToLoad);
        auto & resourcesLoaded = _async ? m_resourcesLoadedAsync : m_resourcesLoaded;

        for (uint i = 0; i < resourcesToLoad.size(); ++i)
        {
            auto & info = resourcesToLoad[0];
            
            if (i == 0)
            {
                VG_PROFILE_CPU("loading");

                auto it = m_resourcesMap.find(info.m_path);
                if (m_resourcesMap.end() == it)
                {
                    loadOneResource(info);
                    resourcesLoaded.push_back(info);
                }
                else
                {
                    info.m_resource->setObject(it->second->getObject());
                    resourcesLoaded.push_back(info);
                }
            }
            else
            {
                m_resourcesToLoad.push_back(resourcesToLoad[i]);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void ResourceManager::loadResourceAsync(Resource * _resource, const string & _path, IObject * _owner)
    {
        m_resourcesToLoad.emplace_back(_resource, _path, _owner);
    }

    //--------------------------------------------------------------------------------------
    void ResourceManager::loadOneResource(ResourceLoadInfo & info)
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

        // Add resources that were loaded async during previous frames
        {
            lock_guard<mutex> lock(m_resourceLoadedAsyncMutex);
            m_resourcesLoadedAsync.push_back(info);
        }        
    }

    //--------------------------------------------------------------------------------------
    void ResourceManager::updateLoading()
    {
        VG_PROFILE_CPU("UpdateLoading");

        static u32 skipFrames = 2;
        if (skipFrames != 0)
        {
            --skipFrames;
            return;
        }
        
        #if VG_RESOURCE_MANAGER_ASYNC_LOADING
        // Add resources that were loaded async during previous frames
        {
            lock_guard<mutex> lock(m_resourceLoadedAsyncMutex);

            const auto asyncLoadCount = m_resourcesLoadedAsync.size();

            if (asyncLoadCount > 0)
            {
                m_resourcesLoaded.reserve(m_resourcesLoaded.size() + asyncLoadCount);

                for (uint i = 0; i < asyncLoadCount; ++i)
                    m_resourcesLoaded.push_back(m_resourcesLoadedAsync[i]);

                m_resourcesLoadedAsync.clear();
            }
        }
        #endif

        // Sync point to notify resource loaded last frame
        for (uint i = 0; i < m_resourcesLoaded.size(); ++i)
        {
            const auto & info = m_resourcesLoaded[i];
            info.m_owner->onResourceLoaded(info.m_resource);
        }
        m_resourcesLoaded.clear();

        #if !VG_RESOURCE_MANAGER_ASYNC_LOADING
        updateLoading(false);
        #endif
    }
}