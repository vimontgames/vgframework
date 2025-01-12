#include "ResourceManager.h"
#include "core/Resource/Resource.h"
#include "core/Timer/Timer.h"
#include "core/File/File.h"
#include "core/Scheduler/Scheduler.h"
#include "core/string/string.h"
#include "core/IResourceMeta.h"
#include "engine/EngineOptions.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS(ResourceManager, "Resource Manager");

    //--------------------------------------------------------------------------------------
    bool ResourceManager::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyObjectPtrDictionaryEx(ResourceManager, m_resourceInfosMap, "Resources", PropertyFlags::ReadOnly);

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

        for (auto & pair : m_resourceMeta)
            VG_SAFE_RELEASE(pair.second);
        m_resourceMeta.clear();
    }

    //--------------------------------------------------------------------------------------
    bool ResourceManager::HasResourceLoading() const
    {
        Lock();
        bool hasResourceLoading = m_resourcesToLoad.size() != 0 || m_resourcesLoaded.size() != 0;
        Unlock();

        return hasResourceLoading;
    }

    //--------------------------------------------------------------------------------------
    uint ResourceManager::GetResourceCount() const
    {
        return (uint)m_resourceInfosMap.size();
    }

    //--------------------------------------------------------------------------------------
    const IResourceInfo & ResourceManager::GetResourceInfo(core::uint _index) const
    {
        return *std::next(m_resourceInfosMap.begin(), _index)->second;
    }

    //--------------------------------------------------------------------------------------
    core::IResourceMeta * ResourceManager::GetOrCreateResourceMeta(const IResource * _resource) const
    {
        auto rm = ResourceManager::get();
        auto path = _resource->GetResourcePath();
        
        if (IResourceMeta * meta = rm->GetResourceMeta(path))
        {
            io::FileAccessTime metaLastWrite;
            if (io::getLastWriteTime(io::getMetaPath(path), &metaLastWrite))
            {
                if (metaLastWrite <= meta->GetLastWriteTime())
                    return meta;
            }
        }
        
        IResourceMeta * meta = _resource->CreateResourceMeta(path);

        if (meta)
        {
            // Load if exists
            if (meta->Load(path))
            {
                rm->SetResourceMeta(path, meta);
            }
            else
            {
                // Or create
                rm->SetResourceMeta(path, meta);
                meta->RegisterUID();
                meta->Save(path);
            }

            // Save timestamp in meta
            io::FileAccessTime metaLastWrite;
            if (io::getLastWriteTime(io::getMetaPath(path), &metaLastWrite))
                meta->SetLastWriteTime(metaLastWrite);
        }
            
        return meta;      
    }

    //--------------------------------------------------------------------------------------
    core::IResourceMeta * ResourceManager::GetResourceMeta(const core::string & _resourcePath) const
    {
        auto it = m_resourceMeta.find(_resourcePath);
        if (m_resourceMeta.end() != it)
            return it->second;
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    void ResourceManager::SetResourceMeta(const core::string & _resourcePath, core::IResourceMeta * _meta)
    {
        auto it = m_resourceMeta.find(_resourcePath);
        if (m_resourceMeta.end() != it)
        {
            if (_meta != it->second)
            {
                auto uid = nullptr != it->second ? it->second->GetUID() : 0;
                VG_SAFE_RELEASE(it->second);
                it->second = _meta;
                if (uid)
                {
                    _meta->SetUID(uid);
                    _meta->RegisterUID();
                }
            }
        }
        else
        {
            m_resourceMeta.emplace(_resourcePath, _meta);
        }
    }

    //--------------------------------------------------------------------------------------
    void ResourceManager::UpdateResources(bool _async)
    {
        if (_async)
            m_needUpdateResource = true;
        else
            updateResources();
    }

    //--------------------------------------------------------------------------------------
    core::uint ResourceManager::updateResources()
    {
        uint count = 0;

        // Unloading resources will remove entries in the resourceMap so we need to copy the existing resources to iterate over
        vector<ResourceInfo*> allResourceInfos;
        for (auto pair : m_resourceInfosMap)
        {
            VG_SAFE_INCREASE_REFCOUNT(pair.second);
            allResourceInfos.push_back(pair.second);
        }

        for (const auto & info : allResourceInfos)
        {
            if (CookStatus::UP_TO_DATE != needsCook(*info))
            {
                // Setting resource path to null will remove clients so we need to copy
                vector<core::IResource *> clients = info->m_clients;
                for (auto client : clients)
                    VG_SAFE_INCREASE_REFCOUNT(client);

                // Remove all clients
                for (uint i = 0; i < clients.size(); ++i)
                    clients[i]->ClearResourcePath();

                // Rebind all clients to update the resource
                for (uint i = 0; i < clients.size(); ++i)
                    clients[i]->SetResourcePath(info->m_path);

                for (auto client : clients)
                    VG_SAFE_RELEASE(client);

                count++;
            }
        }

        VG_INFO("[Resource] %u Resource files have been modified", count);

        for (auto info : allResourceInfos)
            VG_SAFE_RELEASE(info);

        return count;
    }

    //--------------------------------------------------------------------------------------
    void ResourceManager::Reimport(core::IResource * _res, bool _async)
    {
        if (_async)
            m_resourcesToReimport.push_back(_res);
        else
            reimport(_res);        
    }

    //--------------------------------------------------------------------------------------
    void ResourceManager::reimport(core::IResource * _res)
    {
        VG_ASSERT(Kernel::getScheduler()->IsMainThread());

        // Unloading resources will remove entries in the resourceMap so we need to copy the existing resources to iterate over
        vector<ResourceInfo *> allResourceInfos;
        for (auto pair : m_resourceInfosMap)
        {
            VG_SAFE_INCREASE_REFCOUNT(pair.second);
            allResourceInfos.push_back(pair.second);
        }

        for (auto info : allResourceInfos)
        {
            const string & path = info->m_path;
            if (!path.empty() && path == _res->GetResourcePath())
            {
                VG_INFO("[Resource] Reimport \"%s\" (%u clients)", path.c_str(), info->m_clients.size());

                // Setting resource path to null will remove clients so we need to copy
                vector<core::IResource *> clients = info->m_clients;
                for (auto client : clients)
                    VG_SAFE_INCREASE_REFCOUNT(client);

                // Remove all clients
                for (uint i = 0; i < clients.size(); ++i)
                    clients[i]->ClearResourcePath();

                // Rebind all clients to update the resource
                for (uint i = 0; i < clients.size(); ++i)
                    clients[i]->SetResourcePath(path);

                auto it = m_resourceInfosMap.find(path);
                VG_ASSERT(m_resourceInfosMap.end() != it);
                if (m_resourceInfosMap.end() != it)
                {
                    ResourceInfo * info = it->second;
                    info->m_forceReimport = true;
                }

                for (auto client : clients)
                    VG_SAFE_RELEASE(client);
            }
        }

        for (auto info : allResourceInfos)
            VG_SAFE_RELEASE(info);
    }

    //--------------------------------------------------------------------------------------
    void ResourceManager::flushPendingLoading()
    {
        if (true == m_isLoadingThreadRunning.exchange(false))
            m_loadingThread.join();
    }

    //--------------------------------------------------------------------------------------
    void ResourceManager::registerThread()
    {
        if (!m_isLoadingThreadRegistered)
        {
            Kernel::getScheduler()->RegisterCurrentThread("Loading", ThreadType::Loading, 0, 1);
            m_isLoadingThreadRegistered = true;
        }
    }

    //--------------------------------------------------------------------------------------
    void ResourceManager::loading(ResourceManager * _this)
    {
        _this->registerThread();
        

        while (_this->isLoadingThreadRunning())
        {
            _this->updateLoadingThread();
            Sleep(0);
        }
    }

    //--------------------------------------------------------------------------------------
    void ResourceManager::flushUpdateResource()
    {
        if (m_needUpdateResource)
        {
            updateResources();
            m_needUpdateResource = false;
        }
    }

    //--------------------------------------------------------------------------------------
    void ResourceManager::updateLoadingThread()
    {
        VG_ASSERT(Kernel::getScheduler()->IsLoadingThread());
        
        IResource * res = nullptr;

        // Is there any resource to load?
        if (m_resourcesToLoad.size() > 0)
        {
            lock_guard lock(m_addResourceToLoadRecursiveMutex);
            res = m_resourcesToLoad.size() > 0 ? m_resourcesToLoad[0] : nullptr;
        }

        // Load resource without blocking mutex
        if (nullptr != res)
        {
            VG_PROFILE_CPU("loading");
            string resPath = res->GetResourcePath();

            auto it = m_resourceInfosMap.find(resPath);
            VG_ASSERT(m_resourceInfosMap.end() != it);
            auto & info = it->second;
            VG_ASSERT(info->m_object == nullptr);
            loadOneResource(*info);
            m_resourcesLoadedAsync.push_back(res);
        }

        // Remove loaded resource from list
        if (nullptr != res)
        {
            lock_guard lock(m_addResourceToLoadRecursiveMutex);
            VG_VERIFY(vector_helper::remove(m_resourcesToLoad, res));
        }
    }

    //--------------------------------------------------------------------------------------
    void ResourceManager::LoadResourceAsync(IResource * _resource, const core::string & _oldPath, const string & _newPath)
    {
        VG_ASSERT(Kernel::getScheduler()->IsMainThread() || Kernel::getScheduler()->IsLoadingThread());
        VG_ASSERT(_resource->GetParent() != nullptr); 
        VG_ASSERT(_resource->GetResourcePath() == _newPath); // TODO: get rid of the '_newPath' parameter?

        const auto * options = EngineOptions::get();

        // unload previous
        {
            auto it = m_resourceInfosMap.find(_oldPath);
            if (m_resourceInfosMap.end() != it)
                UnloadResource(_resource, _oldPath);
        }

        if (io::exists(_newPath))
        {
            // Reuse existing object if it's already loaded
            auto it = m_resourceInfosMap.find(_resource->GetResourcePath());
            if (m_resourceInfosMap.end() != it)
            {
                ResourceInfo & loaded = *it->second;

                VG_ASSERT(loaded.GetResourceType() == _resource->GetClassName(), "[Resource] Resource \"%s\" is already loaded as a '%s' resource and cannot also be loaded as a `%s`' resource", _newPath.c_str(), loaded.GetResourceType().c_str(), _resource->GetClassName());
                
                // TODO : make resource type part of the hash and cooked name to that we e.g., can have a 'MeshResource' and an 'AnimationResource' from the same FBX file?
                if (loaded.GetResourceType() == _resource->GetClassName())
                {
                    // Add to client list
                    loaded.m_clients.push_back(_resource);

                    if (loaded.m_object != nullptr)
                    {
                        // already loaded? Add to resource to update BUT is several clients requested the resource at the same frame it could be not ready yet and dropped :(
                        m_resourcesLoadedAsync.push_back(_resource);
                    }
                }
            }
            else
            {
                // Create entry in ResourceMap
                string name = io::getFileName(_resource->GetResourcePath());
                ResourceInfo * info = new ResourceInfo(name);
                info->m_clients.push_back(_resource);
                info->m_path = _newPath;
                info->m_object = nullptr;

                m_resourceInfosMap.insert(make_pair(_newPath, info));

                // Add resource to load asynchronously
                {
                    lock_guard lock(m_addResourceToLoadRecursiveMutex);

                    if (Scheduler::get()->IsMainThread())
                        m_resourcesToLoad.emplace_back(_resource);
                    else
                        m_resourcesToLoadAsync.emplace_back(_resource);
                }
            }
        }
        else
        {
            if (!_newPath.empty())
            {
                auto * parentGameObject = _resource->GetParentGameObject();
                if (nullptr != parentGameObject)
                    VG_ERROR("[Resource] Could not find %s file \"%s\" referenced by GameObject \"%s\"", _resource->GetClassName(), _newPath.c_str(), parentGameObject->GetName().c_str());
                else
                    VG_ERROR("[Resource] Could not find %s file \"%s\"", _resource->GetClassName(), _newPath.c_str());
            }
        }

        //flushResourcesToLoadAsync();
    }

    //--------------------------------------------------------------------------------------
    // Resource Shared Object is released after the last client is unloaded
    //--------------------------------------------------------------------------------------
    void ResourceManager::UnloadResource(core::IResource * _resource, const core::string & _path)
    {
        auto it = m_resourceInfosMap.find(_path);
        if (m_resourceInfosMap.end() != it)
        {
            ResourceInfo * info = it->second;
            VG_VERIFY(vector_helper::remove(info->m_clients, (IResource*)_resource));

            // Resource has no more owners, delete it
            if (info->m_clients.size() == 0)
            {
                VG_SAFE_RELEASE(info->m_object);
                m_resourceInfosMap.erase(it);
                VG_SAFE_RELEASE(info);
                _resource->Unload(_path);
            }
            
            _resource->UnloadSubResources();
            _resource->GetParent()->OnResourceUnloaded(_resource);  
            _resource->SetObject(nullptr);
        }
    }

    //--------------------------------------------------------------------------------------
    CookStatus ResourceManager::needsCook(const ResourceInfo & _info)
    {
        const auto & resourcePath = _info.GetResourcePath();
        const string cookedPath = io::getCookedPath(resourcePath);

        io::FileAccessTime rawDataLastWrite;
        if (io::getLastWriteTime(resourcePath, &rawDataLastWrite))
        {
            if (io::exists(cookedPath))
            {
                io::FileAccessTime cookedFilelastWrite;
                if (io::getLastWriteTime(cookedPath, &cookedFilelastWrite))
                {
                    if (cookedFilelastWrite < rawDataLastWrite)
                        return CookStatus::RAWDATA_FILE_UPDATED;

                    io::FileAccessTime metaFilelastWrite;
                    const string metaPath = io::getMetaPath(resourcePath);
                    if (io::getLastWriteTime(metaPath, &metaFilelastWrite))
                    {
                        if (cookedFilelastWrite < metaFilelastWrite)
                            return CookStatus::META_FILE_UPDATED;
                    }
                }
            }
            else
            {
                return CookStatus::NO_COOKED_FILE;
            }
        }
        return CookStatus::UP_TO_DATE;
    }

    //--------------------------------------------------------------------------------------
    void ResourceManager::loadOneResource(ResourceInfo & _info)
    {
        VG_ASSERT(Kernel::getScheduler()->IsLoadingThread());

        // get shared resource info
        auto & clients = _info.m_clients;
        const string & path = _info.m_path;

        VG_PROFILE_CPU(path.c_str());

        // check for an up-to-date cooked version of the resource
        auto needCook = needsCook(_info);

        if (CookStatus::UP_TO_DATE == needCook)
        {
            if (_info.m_forceReimport)
                needCook = CookStatus::FORCE_REIMPORT;
        }

        // Reimport is forced only once
        _info.m_forceReimport = false;

        // HACK: use 1st client
        auto * cooker = clients[0];

        bool done = false;

        while (!done)
        {
            const auto startCook = Timer::getTick();
            if (CookStatus::UP_TO_DATE != needCook)
            {
                VG_PROFILE_CPU("Cook");
                VG_INFO("[Resource] File \"%s\" needs cook because %s", path.c_str(), asString(needCook).c_str());

                bool isFileCooked = cooker->Cook(path);

                VG_ASSERT(isFileCooked, "Could not cook file \"%s\"", path.c_str());

                if (isFileCooked)
                {
                    const string cookFile = io::getCookedPath(path);

                    if (io::setLastWriteTime(cookFile, io::getCurrentFileTime()))
                        VG_INFO("[Resource] File \"%s\" cooked in %.2f ms", path.c_str(), Timer::getEnlapsedTime(startCook, Timer::getTick()));
                }
            }
;
            const auto startLoad = Timer::getTick();

            VG_ASSERT(_info.m_object == nullptr);
            string path = cooker->GetResourcePath();

            VG_ASSERT(!path.empty());

            // Cooked file may seem up to date but format version actually changed
            _info.m_object = cooker->Load(path);

            if (nullptr != _info.m_object)
            {
                VG_INFO("[Resource] File \"%s\" loaded in %.2f ms", path.c_str(), Timer::getEnlapsedTime(startLoad, Timer::getTick()));
                done = true;
            }
            else
            {
                if (CookStatus::UP_TO_DATE == needCook)
                {
                    needCook = CookStatus::COOK_VERSION_DEPRECATED;
                }
                else
                {
                    VG_ERROR("[Resource] Could not load File \"%s\"", path.c_str());
                    done = true;
                }
            }
        }       
    }

    //--------------------------------------------------------------------------------------
    void ResourceManager::flushResourceToReimport()
    {
        VG_PROFILE_CPU("flushResourceToReimport");

        for (uint i = 0; i < m_resourcesToReimport.size(); ++i)
            reimport(m_resourcesToReimport[i]);
        m_resourcesToReimport.clear();
    }

    //--------------------------------------------------------------------------------------
    void ResourceManager::flushResourcesToLoadAsync()
    {
        VG_PROFILE_CPU("flushResourcesToLoadAsync");

        lock_guard lock(m_addResourceToLoadRecursiveMutex);

        const auto asyncToLoadCount = m_resourcesToLoadAsync.size();

        if (asyncToLoadCount > 0)
        {
            m_resourcesToLoad.reserve(m_resourcesToLoad.size() + asyncToLoadCount);

            for (uint i = 0; i < asyncToLoadCount; ++i)
                m_resourcesToLoad.push_back(m_resourcesToLoadAsync[i]);

            m_resourcesToLoadAsync.clear();

            const auto * options = EngineOptions::get();

            if (options->useResourceLoadingPriority())
            {
                VG_PROFILE_CPU("resourceLoadingPriority");

                sort(m_resourcesToLoad.begin(), m_resourcesToLoad.end(), [](IResource * a, IResource * b)
                {
                    return (a->GetClassDesc()->GetPriority() < b->GetClassDesc()->GetPriority());
                }
                );
            
                #if VG_DEBUG
                VG_DEBUGPRINT("\n");
                VG_DEBUGPRINT("NEW | INDEX | PRIORITY | TYPE      | PATH\n", m_resourcesToLoad.size());
                VG_DEBUGPRINT("----+-------+----------+-----------+---------------------------------\n");
                for (uint i = 0; i < m_resourcesToLoad.size(); ++i)
                {
                    const auto * res = m_resourcesToLoad[i];
                    string shortTypeName = res->GetClassDesc()->GetClassName();
                    auto resPos = shortTypeName.find("Resource");
                    if (-1 != resPos)
                        shortTypeName = shortTypeName.substr(0, resPos);
                    VG_DEBUGPRINT("%s | %5u | %8i | %9s | \"%s\"\n", "   ", i, res->GetClassDesc()->GetPriority(), shortTypeName.c_str(), res->GetResourcePath().c_str());
                }
                VG_DEBUGPRINT("\n");
                #endif
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void ResourceManager::flushResourcesLoadedAsync()
    {
        VG_PROFILE_CPU("flushResourcesLoadedAsync");
        lock_guard lock(m_resourceLoadedAsyncMutex);

        const auto asyncLoadedCount = m_resourcesLoadedAsync.size();

        if (asyncLoadedCount > 0)
        {
            m_resourcesLoaded.reserve(m_resourcesLoaded.size() + asyncLoadedCount);

            for (uint i = 0; i < asyncLoadedCount; ++i)
                m_resourcesLoaded.push_back(m_resourcesLoadedAsync[i]);

            m_resourcesLoadedAsync.clear();
        }
    }

    //--------------------------------------------------------------------------------------
    void ResourceManager::updateMainThread()
    {
        VG_PROFILE_CPU("Loading");
        VG_ASSERT(Kernel::getScheduler()->IsMainThread());

        // Update resources to reimport
        flushResourceToReimport();

        // Add resource to load added asynchronously
        flushResourcesToLoadAsync();
        
        // Add resources that were loaded async during previous frames
        flushResourcesLoadedAsync();

        // Sync point to notify resource loaded last frame
        for (uint i = 0; i < m_resourcesLoaded.size(); ++i)
        {
            const auto & res = m_resourcesLoaded[i];

            auto it = m_resourceInfosMap.find(res->GetResourcePath());
            auto & info = it->second;

            // Set Shared Resource Object and Notify owner
            res->SetObject(info->m_object);
            res->LoadSubResources();
            IObject * resOwner = res->GetParent();
            VG_ASSERT(nullptr != resOwner);
            if (nullptr != resOwner)
                resOwner->OnResourceLoaded(res);
        }
        m_resourcesLoaded.clear();

        // Update null entries in resources (e.g., several requests for the same resource object)
        for (auto pair : m_resourceInfosMap)
        {
            ResourceInfo * info = pair.second;
            auto & clients = info->m_clients;
            for (uint i = 0; i < clients.size(); ++i)
            {
                auto & res = clients[i];
                if (res->GetObject() == nullptr)
                {
                    if (nullptr != info->m_object)
                    {
                        res->SetObject(info->m_object);
                        res->LoadSubResources();
                        res->GetParent()->OnResourceLoaded(res);
                    }
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void ResourceManager::Lock() const
    {
        m_addResourceToLoadRecursiveMutex.lock();
        m_resourceLoadedAsyncMutex.lock();
    }

    //--------------------------------------------------------------------------------------
    void ResourceManager::Unlock() const
    {
        m_addResourceToLoadRecursiveMutex.unlock();
        m_resourceLoadedAsyncMutex.unlock();
    }
}