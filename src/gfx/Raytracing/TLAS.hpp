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
            BindlessTable * bindlessTable = gfx::Device::get()->getBindlessTable();
            m_bindlessTLASHandle = bindlessTable->allocBindlessTLASHandle(nullptr);
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

        BindlessTable * bindlessTable = gfx::Device::get()->getBindlessTable();
        if (m_bindlessTLASHandle.isValid())
            bindlessTable->freeBindlessTLASHandle(m_bindlessTLASHandle);
    }

    //--------------------------------------------------------------------------------------
    const BindlessTLASHandle & TLAS::getHandle() const
    {
        return m_bindlessTLASHandle;
    }
}