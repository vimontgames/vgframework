namespace vg::graphics::driver::dx12
{
    //--------------------------------------------------------------------------------------
    Buffer::Buffer(const BufferDesc & _bufDesc, const core::string & _name, void * _initData) :
        base::Buffer(_bufDesc, _name, _initData)
    {
        auto * device = driver::Device::get();
        auto * d3d12device = device->getd3d12Device();        
    }

    //--------------------------------------------------------------------------------------
    Buffer::~Buffer()
    {
        auto * device = driver::Device::get();
    }
}