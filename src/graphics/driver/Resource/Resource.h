#pragma once

#include "Resource_consts.h"

namespace vg::graphics::driver
{
    namespace base
    {
        class Resource
        {
        public:
            enum class ResourceType : core::u8
            {
                Texture = 0,
                Buffer
            };

        protected:
            ResourceType m_resourceType;
        };
    }
}

#include VG_GFXAPI_HEADER(Resource)

namespace vg::graphics::driver
{
    class Resource : public VG_GFXAPI::Resource
    {
    };
}