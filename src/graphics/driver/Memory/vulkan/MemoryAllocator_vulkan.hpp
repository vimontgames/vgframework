namespace vg::graphics::driver::vulkan
{
    //--------------------------------------------------------------------------------------
    MemoryAllocator::MemoryAllocator() :
        super::MemoryAllocator()
    {
        auto * device = driver::Device::get();
        VG_ASSERT(device);
    }

    //--------------------------------------------------------------------------------------
    MemoryAllocator::~MemoryAllocator()
    {

    }
}