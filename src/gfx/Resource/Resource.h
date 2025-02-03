#pragma once

#include "Resource_consts.h"
#include "core/Object/Object.h"

vg_enum_class_ns(vg::gfx, ResourceFlags, core::u32,
    None = 0x00000000,
    Backbuffer = 0x00000001
);

vg_enum_class_ns(vg::gfx, ResourceType, core::u8,
    Texture = 0,
    Buffer
);

namespace vg::gfx
{
    namespace base
    {
        class Resource : public core::Object
        {
            using super = core::Object;

        public:
            VG_INLINE void setResourceFlags(ResourceFlags _resourceFlags)
            {
                m_flags = _resourceFlags;
            }

            VG_INLINE bool testResourceFlags(ResourceFlags _resourceFlags) const 
            { 
                return 0 != (std::underlying_type<ResourceFlags>::type(_resourceFlags) & std::underlying_type<ResourceFlags>::type(m_flags));
            }


        protected:
            ResourceType m_resourceType;
            ResourceFlags m_flags = ResourceFlags::None;
        };
    }
}

#include VG_GFXAPI_HEADER(Resource)

namespace vg::gfx
{
    class Resource : public VG_GFXAPI::Resource
    {
    public:
        const char * GetClassName() const final { return "Resource"; }
    };
}