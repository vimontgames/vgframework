#include "gfx/Precomp.h"
#include "MemoryAllocator.h"
#include "gfx/Device/Device.h"

#ifdef VG_DX12
#include "dx12/MemoryAllocator_dx12.hpp"
#elif defined(VG_VULKAN)
#include "vulkan/MemoryAllocator_vulkan.hpp"
#else
#error Undefined GFXAPI
#endif

using namespace vg::core;

namespace vg::gfx
{
    namespace base
    {
        //--------------------------------------------------------------------------------------
        MemoryAllocator::MemoryAllocator()
        {

        }

        //--------------------------------------------------------------------------------------
        MemoryAllocator::~MemoryAllocator()
        {

        }
    }

    //--------------------------------------------------------------------------------------
    MemoryAllocator::MemoryAllocator() :
        super::MemoryAllocator()
    {

    }

    //--------------------------------------------------------------------------------------
    MemoryAllocator::~MemoryAllocator()
    {

    }
}