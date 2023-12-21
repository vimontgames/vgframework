#include "TLAS.h"

#define VG_TLAS_SIZE            (1024 * 1024 * 32)
#define VG_TLAS_SCRATCH_SIZE    (1024 * 1024 * 32)
#define VG_TLAS_INSTANCECOUNT   (1024)

#include VG_GFXAPI_IMPL(TLAS)

namespace vg::gfx
{
    namespace base
    {
        //--------------------------------------------------------------------------------------
        TLAS::TLAS()
        {
            auto device = gfx::Device::get();

      
        }

        //--------------------------------------------------------------------------------------
        TLAS::~TLAS()
        {

        }
    }

    //--------------------------------------------------------------------------------------
    TLAS::TLAS() :
        super()
    {

    }

    //--------------------------------------------------------------------------------------
    TLAS::~TLAS()
    {
        VG_SAFE_RELEASE(m_resultBuffer);
        VG_SAFE_RELEASE(m_scratchBuffer);
        VG_SAFE_RELEASE(m_instanceBuffer);
    }
}