#pragma once
#include "core/Object/Object.h"
#include "RayTracing_consts.h"
#include "gfx/BindlessTable/BindlessTable_consts.h"

namespace vg::gfx
{
    namespace base
    {
        class TLAS : public core::Object
        {
        public:
            TLAS();
            ~TLAS();

            gfx::BindlessTLASHandle m_bindlessTLASHandle;
            gfx::Buffer *           m_resultBuffer = nullptr;
            gfx::Buffer *           m_scratchBuffer = nullptr;
            gfx::Buffer *           m_instanceBuffer = nullptr;
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

        gfx::Buffer *               getBuffer   () const { return m_resultBuffer; }
        const BindlessTLASHandle &  getHandle   () const;

        TLAS();
        ~TLAS();
    };
}