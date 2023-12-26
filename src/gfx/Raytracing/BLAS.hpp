#include "BLAS.h"

#include VG_GFXAPI_IMPL(BLAS)

namespace vg::gfx
{
    namespace base
    {
        //--------------------------------------------------------------------------------------
        BLAS::BLAS(BLASUpdateType _blasUpdateType) :
            m_blasUpdateType(_blasUpdateType)
        {

        }

        //--------------------------------------------------------------------------------------
        BLAS::~BLAS()
        {
            
        }
    }

    //--------------------------------------------------------------------------------------
    BLAS::BLAS(BLASUpdateType _blasUpdateType) :
        super(_blasUpdateType)
    {

    }

    //--------------------------------------------------------------------------------------
    BLAS::~BLAS()
    {
        VG_SAFE_RELEASE(m_resultBuffer);
        VG_SAFE_RELEASE(m_scratchBuffer);
    }
}