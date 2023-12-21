#pragma once
#include "core/Object/Object.h"
#include "RayTracing_consts.h"

namespace vg::gfx
{
    namespace base
    {
        class TLAS : public core::Object
        {
        public:
            TLAS();
            ~TLAS();

            gfx::Buffer * m_resultBuffer = nullptr;
            gfx::Buffer * m_scratchBuffer = nullptr;
            gfx::Buffer * m_instanceBuffer = nullptr;
        };
    }
}

#include VG_GFXAPI_HEADER(TLAS)

namespace vg::gfx
{
    class CommandList;

    class TLAS : public VG_GFXAPI::TLAS
    {
    public:
        using super = VG_GFXAPI::TLAS;
        const char * getClassName() const final { return "TLAS"; }

        TLAS();
        ~TLAS();
    };
}