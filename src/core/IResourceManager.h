#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class IResource;
    class IResourceMeta;

    class IResourceInfo : public core::Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IResourceInfo, core::Object);

        virtual const core::string &    GetResourcePath () const = 0;
        virtual const core::string      GetResourceType () const = 0;
        virtual const core::uint        GetClientCount  () const = 0;
        virtual const core::IResource * GetClient       (core::uint _index) const = 0;
    };

    class IResourceManager : public Object
    {
    public:
        using super = core::Object;

        IResourceManager(const core::string & _name, core::IObject * _parent) :
            super(_name, _parent)
        {

        }

        // Lock during editing
        virtual void Lock() const = 0;
        virtual void Unlock() const = 0;

        // Load/unload resource
        virtual void LoadResourceAsync(core::IResource * _resource, const core::string & _oldPath, const core::string & _path) = 0;
        virtual void UnloadResource(core::IResource * _resource, const core::string & _path) = 0;

        // Return 'true' if any resource is currently loading 
        virtual bool HasAnyLoadingInProgress() const = 0;
        virtual core::uint GetLoadingInProgressCount() const = 0;
        
        // Get number of registered resources
        virtual uint GetResourceCount() const = 0;

        // Get infos about a loaded (or requested resource) by index (slow)
        virtual const IResourceInfo & GetResourceInfo(uint _index) const = 0;

        // Returns a read-only dictionary of resources 
        virtual const core::dictionary<IResourceInfo *> & GetAllResourceInfos() const = 0;

        // Get or create ResourceMeta and update Timestamps
        virtual IResourceMeta * GetOrCreateResourceMeta(const IResource * _resource) const = 0;

        // Get ResourceMeta from path if it already exists
        virtual IResourceMeta * GetResourceMeta(const string & _resourcePath) const = 0;

        // Add or update ResourceMeta
        virtual void SetResourceMeta(const string & _resourcePath, IResourceMeta * _meta) = 0;

        // Reimport modified resources
        virtual void UpdateResources(bool _async = true) = 0;

        // Force reimport specific resource
        virtual void Reimport(IResource * _res, bool _async = true) = 0;

        // Swap two client pointers
        virtual void SwapResources(IResource * _resA, IResource * _resB) = 0;
    };
}