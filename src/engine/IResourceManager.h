#pragma once

#include "core/Object/Object.h"

namespace vg::engine
{
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

        // Reimport modified resources
        virtual core::uint UpdateResources() = 0;
    };
}