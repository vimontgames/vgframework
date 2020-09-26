namespace vg::graphics::driver::vulkan
{
    //--------------------------------------------------------------------------------------
    BindlessTable::BindlessTable() :
        super::BindlessTable()
    {
        auto * device = driver::Device::get();
        VG_ASSERT(device);


    }

    //--------------------------------------------------------------------------------------
    BindlessTable::~BindlessTable()
    {

    }
}