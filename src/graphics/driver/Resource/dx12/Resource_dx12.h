#pragma once

#include "graphics/driver/Resource/Resource.h"

namespace D3D12MA
{
    class Allocation;
}

namespace vg::graphics::driver::dx12
{
    class Resource : public base::Resource
    {
        using super = base::Resource;

    public:
        Resource();
        ~Resource();

        void								setd3d12TextureResource(ID3D12Resource * _resource, D3D12MA::Allocation * _alloc);
        ID3D12Resource *					getd3d12TextureResource() const;

        void								setd3d12BufferResource(ID3D12Resource * _resource, D3D12MA::Allocation * _alloc);
        ID3D12Resource *					getd3d12BufferResource() const;

        Map                                 map(core::uint _subResource = 0, Range _read = Range());
        void                                unmap(core::uint _subResource = 0, Range _write = Range());

    private:
        ID3D12Resource *					m_d3d12resource = nullptr;
        D3D12MA::Allocation *               m_d3d12ma_alloc = nullptr;
    };
}