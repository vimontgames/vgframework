#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class IResource;
}

namespace vg::engine
{
    class IResourceInfo : public core::Object
    {
    public:
        VG_CLASS_VIRTUAL(IResourceInfo, core::Object);

        virtual const core::string &    GetResourcePath () const = 0;
        virtual const core::string      GetResourceType () const = 0;
        virtual const core::uint        GetClientCount  () const = 0;
        virtual const core::IResource * GetClient       (core::uint _index) const = 0;
    };

    class IResourceManager : public core::Object
    {
    public:
        using super = core::Object;

        IResourceManager(const core::string & _name, core::IObject * _parent) :
            super(_name, _parent)
        {

        }

        // Return 'true' if any resource is currently loading 
        virtual bool HasResourceLoading() const = 0;
        
        // Get number of registered resources
        virtual core::uint GetResourceCount() const = 0;

        // Get infos about a loaded (or requested resource)
        virtual const IResourceInfo & GetResourceInfo(core::uint _index) const = 0;

        // Reimport modified resources
        virtual core::uint UpdateResources() = 0;

        // Force reimport specific resource
        virtual void Reimport(core::IResource * _res) = 0;
    };
}