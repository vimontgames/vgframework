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
        BLAS(BLASUpdateType _blasUpdateType, BLASVariantKey _key);
        ~BLAS();

        void addIndexedGeometry(const gfx::Buffer * _ib, core::uint _ibOffset, core::uint _indexCount, const gfx::Buffer * _vb, core::uint _vbOffset, core::uint _vertexCount, core::uint _vbStride, SurfaceType _surfaceType);
        void clear();
        void init(bool _update = false);
        void build(gfx::CommandList * _cmdList, bool _update = false);
        void update(gfx::CommandList * _cmdList);

    private:
        core::vector<D3D12_RAYTRACING_GEOMETRY_DESC>            m_DXRGeometries = {};
        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS    m_DXRAccelStructInputs = {};
    };
}