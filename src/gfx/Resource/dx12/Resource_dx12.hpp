namespace vg::gfx::dx12
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
    void Resource::SetName(const core::string & _name)
    {
        super::SetName(_name);

        if (m_d3d12resource)
        {
            wstring name = wstring_convert(_name);
            m_d3d12resource->SetName(name.c_str());
        }
    }

    //--------------------------------------------------------------------------------------
    void Resource::setd3d12TextureResource(const string & _name, ID3D12Resource * _resource, D3D12MA::Allocation * _alloc)
    {
        m_resourceType = ResourceType::Texture;
        m_d3d12resource = _resource;
        m_d3d12ma_alloc = _alloc;

        SetName(_name);
    }
 
    //--------------------------------------------------------------------------------------
    ID3D12Resource * Resource::getd3d12TextureResource() const
    {
        VG_ASSERT(ResourceType::Texture == m_resourceType);
        return m_d3d12resource;
    }

    //--------------------------------------------------------------------------------------
    void Resource::setd3d12BufferResource(const string & _name, ID3D12Resource * _resource, D3D12MA::Allocation * _alloc)
    {
        m_resourceType = ResourceType::Buffer;
        m_d3d12resource = _resource;
        m_d3d12ma_alloc = _alloc;

        SetName(_name);
    }

    //--------------------------------------------------------------------------------------
    ID3D12Resource * Resource::getd3d12BufferResource() const
    {
        VG_ASSERT(ResourceType::Buffer == m_resourceType);
        return m_d3d12resource;
    }

    //--------------------------------------------------------------------------------------
    Map Resource::map(core::uint _subResource, Range _read)
    {
        Map result;
        const D3D12_RANGE readRange = { _read.begin, _read.end };
        VG_VERIFY_SUCCEEDED(m_d3d12resource->Map(_subResource, &readRange, (void**)&result.data));
        return result;
    }

    //--------------------------------------------------------------------------------------
    void Resource::unmap(core::uint _subResource, Range _write)
    {
        const D3D12_RANGE writeRange = { _write.begin, _write.end };
        m_d3d12resource->Unmap(_subResource, &writeRange);
    }
}