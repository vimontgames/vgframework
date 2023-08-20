#include "renderer/Precomp.h"
#include "MeshInstance.h"

#include "core/Object/AutoRegisterClass.h"

#include "gfx/CommandList/CommandList.h"
#include "gfx/Resource/Buffer.h"
#include "gfx/Resource/Texture.h"

#include "renderer/Renderer.h"
#include "renderer/Model/Mesh/MeshModel.h"
#include "renderer/Model/Material/MaterialModel.h"
#include "renderer/Geometry/Mesh/MeshGeometry.h"
#include "renderer/RenderPass/RenderContext.h"

#include "shaders/system/rootConstants3D.hlsli"

using namespace vg::core;
using namespace vg::gfx;

namespace vg::renderer
{
    VG_AUTO_REGISTER_CLASS(MeshInstance);

    //--------------------------------------------------------------------------------------
    bool MeshInstance::registerClass(core::IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassHelper(MeshInstance, "Mesh Instance", core::IClassDesc::Flags::Instance))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MeshInstance::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    MeshInstance::MeshInstance(const core::string & _name, core::IObject * _parent) :
        IMeshInstance(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    MeshInstance::~MeshInstance()
    {

    }

    //--------------------------------------------------------------------------------------
    void MeshInstance::Draw(const RenderContext & _renderContext, gfx::CommandList * _cmdList) const
    {
        auto * renderer = Renderer::get();

        const MeshModel * model = (MeshModel *)getModel(Lod::Lod0);
        if (nullptr != model)
        {
            const MeshGeometry * geo = model->getGeometry();
                        
            const float4x4 world = getWorldMatrix();
            const float4x4 wvp = mul(world, _renderContext.m_viewProj);
            
            Buffer * vb = geo->getVertexBuffer();
            Buffer * ib = geo->getIndexBuffer();
            _cmdList->setIndexBuffer(ib);

            RootConstants3D root3D;
            root3D.mat = transpose(wvp);
            root3D.setBufferHandle(vb->getBindlessSRVHandle());
            root3D.setFlags(FLAG_ALBEDOMAPS | FLAG_NORMALMAPS);
            root3D.setMode(MODE_DEFAULT);
            root3D.color = getColor();

            _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleList);
            
            const auto & batches = geo->batches();
            for (uint i = 0; i < batches.size(); ++i)
            {
                const auto & batch = batches[i];

                // Setup material 
                const auto * material = model->getMaterial(i);
                if (nullptr != material)
                    material->Setup(_cmdList, &root3D, i);

                _cmdList->setInlineRootConstants(&root3D, RootConstants3DCount);
                _cmdList->drawIndexed(batch.count, batch.offset);
            }
        }
    }
}