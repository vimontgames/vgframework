#pragma once

#include "gfx/Raytracing/BLAS.h"

namespace vg::gfx
{
    class CommandList;
}

namespace vg::gfx::dx12
{
    class BLAS : public base::BLAS
    {
        using super = base::BLAS;

    public:
        BLAS();
        ~BLAS();

        void addIndexedGeometry(gfx::Buffer * _ib, core::uint _ibOffset, core::uint _indexCount, gfx::Buffer * _vb, core::uint _vbOffset, core::uint _vbStride);
        void init();
        void build(gfx::CommandList * _cmdList);

    private:
        core::vector<D3D12_RAYTRACING_GEOMETRY_DESC>            m_DXRGeometries = {};
        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS    m_DXRAccelStructInputs = {};
    };
}