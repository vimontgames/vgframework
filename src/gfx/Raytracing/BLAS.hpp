#include "BLAS.h"

#ifdef VG_DX12
#include "dx12/BLAS_dx12.hpp"
#elif defined(VG_VULKAN)
#include "vulkan/BLAS_vulkan.hpp"
#else
#error Undefined GFXAPI
#endif

namespace vg::gfx
{
    namespace base
    {
        //--------------------------------------------------------------------------------------
        BLAS::BLAS(BLASUpdateType _blasUpdateType, gfx::BLASVariantKey _key) :
            m_updateType(_blasUpdateType),
            m_key(_key)
        {

        }

        //--------------------------------------------------------------------------------------
        BLAS::~BLAS()
        {
            
        }
    }

    //--------------------------------------------------------------------------------------
    BLAS::BLAS(BLASUpdateType _blasUpdateType, BLASVariantKey _key) :
        super(_blasUpdateType, _key)
    {

    }

    //--------------------------------------------------------------------------------------
    BLAS::~BLAS()
    {
        VG_SAFE_RELEASE(m_resultBuffer);
        VG_SAFE_RELEASE(m_scratchBuffer);
    }
}