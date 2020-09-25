namespace vg::graphics::driver::vulkan
{
    //--------------------------------------------------------------------------------------
    DescriptorTable::DescriptorTable() :
        super::DescriptorTable()
    {
        auto * device = driver::Device::get();
        VG_ASSERT(device);


    }

    //--------------------------------------------------------------------------------------
    DescriptorTable::~DescriptorTable()
    {

    }
}