namespace vg::graphics::driver::vulkan
{
    //--------------------------------------------------------------------------------------
    Buffer::Buffer(const BufferDesc & _bufDesc, const core::string & _name, void * _initData) :
        base::Buffer(_bufDesc, _name, _initData)
    {
        auto * device = driver::Device::get();
        auto & vkDevice = device->getVulkanDevice();
    }

    //--------------------------------------------------------------------------------------
    Buffer::~Buffer()
    {
        auto * device = driver::Device::get();
    }
}