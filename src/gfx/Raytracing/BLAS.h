#pragma once
#include "core/Object/Object.h"
#include "RayTracing_consts.h"

namespace vg::gfx
{
    namespace base
    {
        class BLAS : public core::Object
        {
        public:
            BLAS();
            ~BLAS();

            gfx::Buffer * getBuffer() const { return m_resultBuffer; }

            gfx::Buffer *    m_resultBuffer = nullptr;
            gfx::Buffer *    m_scratchBuffer = nullptr;
        };
    }
}

#include VG_GFXAPI_HEADER(BLAS)

namespace vg::gfx
{
    class CommandList;

    class BLAS : public VG_GFXAPI::BLAS
    {
    public:
        using super = VG_GFXAPI::BLAS;
        const char * getClassName() const final { return "BLAS"; }

        BLAS();
        ~BLAS();
    };
}