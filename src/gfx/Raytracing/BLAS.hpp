#include "BLAS.h"

#include VG_GFXAPI_IMPL(BLAS)

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