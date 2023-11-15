#include "ResourceManager.h"
#include "core/Resource/Resource.h"
#include "core/Timer/Timer.h"
#include "core/File/File.h"
#include "core/Scheduler/Scheduler.h"

using namespace vg::core;

#define VG_RESOURCE_MANAGER_ASYNC_LOADING 1

namespace vg::engine
{
    VG_AUTO_REGISTER_CLASS(ResourceManager);

    //--------------------------------------------------------------------------------------
    bool ResourceManager::registerClass(IFactory & _factory)
    {
        if (IClassDesc * desc = _factory.registerClassHelper(ResourceManager, "Resource Manager", IClassDesc::Flags::None))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool ResourceManager::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerPropertyObjectRefDictionaryHelper(ResourceManager, m_resourcesMap, "Resources", IProperty::Flags::ReadOnly);

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
    bool ResourceManager::HasResourceLoading() const
    {
        return m_resourcesToLoad.size() != 0;
    }

    //--------------------------------------------------------------------------------------
    uint ResourceManager::GetResourceCount() const
    {
        return (uint)m_resourcesMap.size();
    }

    //--------------------------------------------------------------------------------------
    uint ResourceManager::UpdateResources()
    {
        uint count = 0;

        // Unloading resources will remove entries in the resourceMap so we need to copy the existing resources to iterate over
        vector<SharedResource*> allSharedResources;
        for (auto pair : m_resourcesMap)
        {
            VG_SAFE_INCREASE_REFCOUNT(pair.second);
            allSharedResources.push_back(pair.second);
        }

        for (auto shared : allSharedResources)
        {
            if (needsCook(shared->m_path))
            {
                // Setting resource path to null will remove clients so we need to copy
                vector<core::IResource *> clients = shared->m_clients;
                for (auto client : clients)
                    VG_SAFE_INCREASE_REFCOUNT(client);

                for (uint i = 0; i < clients.size(); ++i)
                    clients[i]->SetResourcePath("");
                
                for (uint i = 0; i < clients.size(); ++i)
                    clients[i]->SetResourcePath(shared->m_path);

                for (auto client : clients)
                    VG_SAFE_RELEASE(client);

                count++;
            }
        }

        VG_INFO("[Resource] %u Resource files have been modified", count);

        for (auto shared : allSharedResources)
            VG_SAFE_RELEASE(shared);

        return count;
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

        Kernel::getScheduler()->RegisterCurrentThread("Loading");

        while (_this->isLoadingThreadRunning())
        {
            _this->updateLoading(true);
            Sleep(1);
        }
        #endif
    }

    //--------------------------------------------------------------------------------------
    void ResourceManager::updateLoading(bool _async)
    {
        lock_guard<recursive_mutex> lock(m_addResourceToLoadRecursiveMutex);

        auto resourcesToLoad = std::move(m_resourcesToLoad);
        auto & resourcesLoaded = _async ? m_resourcesLoadedAsync : m_resourcesLoaded;

        // load first resource
        if (resourcesToLoad.count() > 0)
        {
            VG_PROFILE_CPU("loading");
            IResource * res = resourcesToLoad[0];
            string resPath = res->GetResourcePath();

            auto it = m_resourcesMap.find(resPath);
            VG_ASSERT(m_resourcesMap.end() != it);
            auto & shared = it->second;
            VG_ASSERT(shared->m_object == nullptr);
            loadOneResource(*shared);
            resourcesLoaded.push_back(res);
        }

        // load others next frame
        for (uint i = 1; i < resourcesToLoad.count(); ++i)
            m_resourcesToLoad.push_back(resourcesToLoad[i]);
    }

    //--------------------------------------------------------------------------------------
    void ResourceManager::loadResourceAsync(Resource * _resource, const core::string & _oldPath, const string & _newPath)
    {
        VG_ASSERT(_resource->getOwner() != nullptr); 
        VG_ASSERT(_resource->GetResourcePath() == _newPath); // TODO: get rid of the '_newPath' parameter?
        lock_guard<recursive_mutex> lock(m_addResourceToLoadRecursiveMutex);

        if (io::exists(_newPath))
        {
            // Reuse existing object if it's already loaded
            auto it = m_resourcesMap.find(_resource->GetResourcePath());
            if (m_resourcesMap.end() != it)
            {
                SharedResource & loaded = *it->second;

                // Add to client list
                loaded.m_clients.push_back(_resource);

                if (loaded.m_object != nullptr)
                {
                    // already loaded? Add to resource to update BUT is several clients requested the resource at the same frame it could be not ready yet and dropped :(
                    m_resourcesLoadedAsync.push_back(_resource);
                }
            }
            else
            {
                // Create entry in ResourceMap
                string name = io::getFileName(_resource->GetResourcePath());
                SharedResource * shared = new SharedResource(name);
                shared->m_clients.push_back(_resource);
                shared->m_path = _newPath;
                shared->m_object = nullptr;

                m_resourcesMap.insert(make_pair(_newPath, shared));
                m_resourcesToLoad.emplace_back(_resource);
            }
        }
        else
        {
            if (!_newPath.empty())
                VG_WARNING("[Resource] Could not find file \"%s\"", _newPath.c_str());

            unloadResource(_resource, _oldPath);
        }
    }

    //--------------------------------------------------------------------------------------
    // Resource Shared Object is released after the last client is unloaded
    //--------------------------------------------------------------------------------------
    void ResourceManager::unloadResource(core::Resource * _resource, const core::string & _path)
    {
        VG_SAFE_INCREASE_REFCOUNT(_resource);
        auto it = m_resourcesMap.find(_path);
        if (m_resourcesMap.end() != it)
        {
            SharedResource * shared = it->second;
            VG_VERIFY(shared->m_clients.remove(_resource));

            // Resource has no more owners, delete it
            if (shared->m_clients.size() == 0)
            {
                VG_SAFE_RELEASE(shared->m_object);
                m_resourcesMap.erase(it);
                VG_SAFE_RELEASE(shared);
                _resource->unload(_path);
            }
            
            _resource->unloadSubResources();
            _resource->getOwner()->onResourceUnloaded(_resource);
            _resource->setObject(nullptr);                
        }
    }

    //--------------------------------------------------------------------------------------
    bool ResourceManager::needsCook(const core::string & _resourcePath)
    {
        const string cookedPath = io::getCookedPath(_resourcePath);

        io::FileAccessTime rawDataLastWrite;
        if (io::getLastWriteTime(_resourcePath, &rawDataLastWrite))
        {
            if (io::exists(cookedPath))
            {
                io::FileAccessTime cookedFilelastWrite;
                if (io::getLastWriteTime(cookedPath, &cookedFilelastWrite))
                {
                    if (cookedFilelastWrite != rawDataLastWrite)
                        return true;
                }
            }
            else
            {
                return true;
            }
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    void ResourceManager::loadOneResource(SharedResource & _shared)
    {
        // get shared resource info
        auto & clients = _shared.m_clients;
        const string path = _shared.m_path;

        // check for an up-to-date cooked version of the resource
        bool needCook = needsCook(_shared.m_path);
        bool done = false;

        while (!done)
        {
            const auto startCook = Timer::getTick();
            if (needCook)
            {
                VG_WARNING("[Resource] File \"%s\" needs cook.\n", path.c_str());

                // HACK: use 1st client to cook
                bool isFileCooked = clients[0]->cook(path);

                VG_ASSERT(isFileCooked, "Could not cook file \"%s\"", path.c_str());

                if (isFileCooked)
                {
                    const string cookFile = io::getCookedPath(path);

                    io::FileAccessTime rawDataLastWrite;
                    VG_VERIFY(io::getLastWriteTime(path, &rawDataLastWrite));

                    if (io::setLastWriteTime(cookFile, rawDataLastWrite))
                        VG_INFO("[Resource] File \"%s\" cooked in %.2f ms", path.c_str(), Timer::getEnlapsedTime(startCook, Timer::getTick()));
                }
            }

            auto client0 = clients[0];
            const auto startLoad = Timer::getTick();

            VG_ASSERT(_shared.m_object == nullptr);
            string path = client0->GetResourcePath();

            VG_ASSERT(!path.empty());

            // Cooked file may seem up to date but format version actually changed
            _shared.m_object = client0->load(path);

            if (nullptr != _shared.m_object)
            {
                VG_INFO("[Resource] File \"%s\" loaded in %.2f ms", path.c_str(), Timer::getEnlapsedTime(startLoad, Timer::getTick()));
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
                    VG_ERROR("[Resource] Could not load resource \"%s\"", path.c_str());
                    done = true;
                }
            }
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
            const auto & res = m_resourcesLoaded[i];

            auto it = m_resourcesMap.find(res->GetResourcePath());
            auto & shared = it->second;

            // Set Shared Resource Object and Notify owner
            res->setObject(shared->m_object);
            res->loadSubResources();
            IObject * resOwner = res->getOwner();
            VG_ASSERT(nullptr != resOwner);
            if (nullptr != resOwner)
                resOwner->onResourceLoaded(res);
        }
        m_resourcesLoaded.clear();

        // Update null entries in resources (e.g. several requests for the same resource object)
        for (auto pair : m_resourcesMap)
        {
            SharedResource * shared = pair.second;
            auto & clients = shared->m_clients;
            for (uint i = 0; i < clients.size(); ++i)
            {
                auto & res = clients[i];
                if (res->getObject() == nullptr)
                {
                    if (nullptr != shared->m_object)
                    {
                        res->setObject(shared->m_object);
                        res->loadSubResources();
                        res->getOwner()->onResourceLoaded(res);
                    }
                }
            }
        }

        #if !VG_RESOURCE_MANAGER_ASYNC_LOADING
        updateLoading(false);
        #endif
    }
}