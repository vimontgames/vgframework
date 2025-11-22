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

    //--------------------------------------------------------------------------------------
    // Return 'true' if first init, 'false' if update
    //--------------------------------------------------------------------------------------
    bool BLAS::init()
    {
        const bool update = m_initDone;
        super::init(update);
        m_initDone = true;
        return !update;
    }

    //--------------------------------------------------------------------------------------
    void BLAS::build(gfx::CommandList * _cmdList)
    {
        VG_ASSERT(!m_buildDone);
        super::build(_cmdList, false);
        m_buildDone = true;
    }

    //--------------------------------------------------------------------------------------
    void BLAS::update(gfx::CommandList * _cmdList)
    {
        VG_ASSERT(m_buildDone);
        super::build(_cmdList, true);
    }
}