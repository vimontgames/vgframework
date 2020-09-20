namespace vg::graphics::driver::dx12
{
    //--------------------------------------------------------------------------------------
    Resource::Resource()
    {

    }

    //--------------------------------------------------------------------------------------
    Resource::~Resource()
    {
        VG_SAFE_RELEASE(m_d3d12resource);
        VG_SAFE_RELEASE(m_d3d12ma_alloc);
    }

    //--------------------------------------------------------------------------------------
    void Resource::setd3d12TextureResource(ID3D12Resource * _resource, D3D12MA::Allocation * _alloc)
    {
        m_resourceType = ResourceType::Texture;
        m_d3d12resource = _resource;
        m_d3d12ma_alloc = _alloc;
    }
 
    //--------------------------------------------------------------------------------------
    ID3D12Resource * Resource::getd3d12TextureResource() const
    {
        VG_ASSERT(ResourceType::Texture == m_resourceType);
        return m_d3d12resource;
    }

    //--------------------------------------------------------------------------------------
    void Resource::setd3d12BufferResource(ID3D12Resource * _resource, D3D12MA::Allocation * _alloc)
    {
        m_resourceType = ResourceType::Buffer;
        m_d3d12resource = _resource;
        m_d3d12ma_alloc = _alloc;
    }

    //--------------------------------------------------------------------------------------
    ID3D12Resource * Resource::getd3d12BufferResource() const
    {
        VG_ASSERT(ResourceType::Buffer == m_resourceType);
        return m_d3d12resource;
    }
}