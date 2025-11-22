#include "TLAS.h"

#define VG_TLAS_INSTANCECOUNT   (4096)
#define VG_TLAS_SIZE            (VG_TLAS_INSTANCECOUNT * 1024 * 32)
#define VG_TLAS_SCRATCH_SIZE    (VG_TLAS_INSTANCECOUNT * 1024 * 32)

#ifdef VG_DX12
#include "dx12/TLAS_dx12.hpp"
#elif defined(VG_VULKAN)
#include "vulkan/TLAS_vulkan.hpp"
#else
#error Undefined GFXAPI
#endif



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

    //--------------------------------------------------------------------------------------
    void TLAS::reset()
    {
        super::reset();
    }

    //--------------------------------------------------------------------------------------
    void TLAS::build(gfx::CommandList * _cmdList)
    {
        VG_PROFILE_GPU("Build TLAS");
        super::build(_cmdList);
    }

    //--------------------------------------------------------------------------------------
    void TLAS::addInstance(const gfx::BLAS * _blas, const core::float4x4 & _world, const core::u32 _instanceID, TLASInstanceFlags _flags)
    {
        VG_ASSERT(_blas->isInitialized());
        super::addInstance(_blas, _world, _instanceID, _flags);
    }
}