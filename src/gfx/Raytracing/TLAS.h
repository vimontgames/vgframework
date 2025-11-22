#pragma once
#include "core/Object/Object.h"
#include "RayTracing_consts.h"
#include "gfx/BindlessTable/BindlessTable_consts.h"

namespace vg::gfx
{
    vg_enum_class(vg::gfx, TLASInstanceFlags, core::u32,
        Opaque = 0x00000001,
        NotOpaque = 0x00000002
    );

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

#ifdef VG_DX12
#include "dx12/TLAS_dx12.h"
#elif defined(VG_VULKAN)
#include "vulkan/TLAS_vulkan.h"
#else
#error Undefined GFXAPI
#endif

namespace vg::gfx
{
    class CommandList;

    class TLAS : public VG_GFXAPI::TLAS
    {
    public:
        using super = VG_GFXAPI::TLAS;
        const char * GetClassName() const final { return "TLAS"; }

        gfx::Buffer *               getBuffer       () const { return m_resultBuffer; }
        const BindlessTLASHandle &  getHandle       () const;

        void                        reset           ();
        void                        build           (gfx::CommandList * _cmdList);
        void                        addInstance     (const gfx::BLAS * _blas, const core::float4x4 & _world, const core::u32 _instanceID, TLASInstanceFlags _flags);

        TLAS();
        ~TLAS();
    };
}