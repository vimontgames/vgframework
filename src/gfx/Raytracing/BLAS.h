#pragma once
#include "core/Object/Object.h"
#include "RayTracing_consts.h"

namespace vg::gfx
{
    using BLASVariantKey = core::u64;

    namespace base
    {
        class BLAS : public core::Object
        {
        public:
            BLAS(BLASUpdateType _blasUpdateType, gfx::BLASVariantKey _key);
            ~BLAS();

            VG_INLINE BLASUpdateType    getUpdateType   () const { return m_updateType; }
            VG_INLINE BLASVariantKey    getKey          () const { return m_key; }
            VG_INLINE gfx::Buffer *     getBuffer       () const { return m_resultBuffer; }
            VG_INLINE bool              isInitialized   () const { return m_initDone; }
            VG_INLINE void              setInitialized  () { m_initDone = true; }

        protected:
            BLASUpdateType  m_updateType;
            BLASVariantKey  m_key;
            gfx::Buffer *   m_resultBuffer = nullptr;
            gfx::Buffer *   m_scratchBuffer = nullptr;
            bool            m_initDone = false;
        };
    }
}

#ifdef VG_DX12
#include "dx12/BLAS_dx12.h"
#elif defined(VG_VULKAN)
#include "vulkan/BLAS_vulkan.h"
#else
#error Undefined GFXAPI
#endif

namespace vg::gfx
{
    class CommandList;    

    class BLAS : public VG_GFXAPI::BLAS
    {
    public:
        using super = VG_GFXAPI::BLAS;
        const char * GetClassName() const final { return "BLAS"; }

        BLAS(BLASUpdateType _blasUpdateType, BLASVariantKey _key);
        ~BLAS();
    };
}