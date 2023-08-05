#pragma once

#include "Resource_consts.h"
#include "core/Object/Object.h"

namespace vg::gfx
{
    struct Map
    {
        void * data = nullptr;
        core::u32 rowPitch = 0;
        core::u32 slicePitch = 0;
    };

    struct Range
    {
        core::u32 begin = 0;
        core::u32 end = 0;
    };

    namespace base
    {
        class Resource : public core::Object
        {
            using super = core::Object;

        public:
            enum class ResourceType : core::u8
            {
                Texture = 0,
                Buffer,
            };

        protected:
            ResourceType m_resourceType;
        };
    }
}

#include VG_GFXAPI_HEADER(Resource)

namespace vg::gfx
{
    class Resource : public VG_GFXAPI::Resource
    {
    public:
        const char * getClassName() const final { return "Resource"; }
    };
}