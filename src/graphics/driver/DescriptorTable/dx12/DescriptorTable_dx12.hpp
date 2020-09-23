namespace vg::graphics::driver::dx12
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