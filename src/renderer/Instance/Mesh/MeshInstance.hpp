#include "renderer/Precomp.h"
#include "MeshInstance.h"

#include "core/Object/AutoRegisterClass.h"

#include "gfx/CommandList/CommandList.h"
#include "gfx/Resource/Buffer.h"
#include "gfx/Resource/Texture.h"

#include "renderer/Renderer.h"
#include "renderer/Options/DisplayOptions.h"
#include "renderer/Model/Mesh/MeshModel.h"
#include "renderer/Model/Material/MaterialModel.h"
#include "renderer/Geometry/Mesh/MeshGeometry.h"
#include "renderer/Animation/Skeleton.h"
#include "renderer/RenderPass/RenderContext.h"
#include "renderer/DebugDraw/DebugDraw.h"

#include "shaders/system/rootConstants3D.hlsli"

#define HLSLPP_FEATURE_TRANSFORM
#include "D:\GitHub\vimontgames\vgframework\extern\hlslpp\include\transform\hlsl++_transform_float4x4.h"

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
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    MeshInstance::~MeshInstance()
    {
        VG_SAFE_RELEASE(m_instanceSkeleton);
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

            Buffer * vb = geo->getVertexBuffer();
            Buffer * ib = geo->getIndexBuffer();
            _cmdList->setIndexBuffer(ib);

            RootConstants3D root3D;
            root3D.mat = transpose(world);
            root3D.setBufferHandle(vb->getBufferHandle());
            
            u16 flags = 0;
            root3D.setFlags(flags);

            VertexFormat vertexFormat = geo->getVertexFormat();
            root3D.setVertexFormat(vertexFormat);

            auto pickingID = getPickingID();
            root3D.setPickingID(pickingID);

            _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleList);

            const auto & batches = geo->batches();
            for (uint i = 0; i < batches.size(); ++i)
            {
                const auto & batch = batches[i];

                // Setup material 
                const MaterialModel * material = getMaterial(i);
                if (nullptr != material)
                    material->Setup(_renderContext, _cmdList, &root3D, i);
                else
                    renderer->getDefaultMaterial()->Setup(_renderContext, _cmdList, &root3D, i);

                root3D.color *= getColor();

                _cmdList->setGraphicRootConstants(0, (u32 *)&root3D, RootConstants3DCount);
                _cmdList->drawIndexed(batch.count, batch.offset);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void MeshInstance::SetModel(Lod _lod, IModel * _model)
    {
        MeshModel * meshModel = dynamic_cast<MeshModel *>(_model);
        VG_ASSERT(nullptr == _model || nullptr != meshModel);

        super::SetModel(_lod, (Model *)_model);

        // Copy skeleton if any
        if (nullptr != meshModel)
        {
            const Skeleton * modelSkeleton = meshModel->getSkeleton();
            if (m_instanceSkeleton != modelSkeleton)
                setInstanceSkeleton(modelSkeleton);
        }
        else
        {
            setInstanceSkeleton(nullptr);
        }
    }

    //--------------------------------------------------------------------------------------
    bool MeshInstance::setInstanceSkeleton(const Skeleton * _skeleton)
    {
        if (nullptr == m_instanceSkeleton && nullptr == _skeleton)
            return false;

        VG_SAFE_RELEASE(m_instanceSkeleton);

        if (nullptr != _skeleton)
        {
            m_instanceSkeleton = new Skeleton("InstanceSkeleton", this);
            m_instanceSkeleton->m_nodes = _skeleton->getNodes();
            m_instanceSkeleton->m_boneIndices = _skeleton->getBoneIndices();
            m_instanceSkeleton->m_boneMatrices = _skeleton->getBoneMatrices();                
        }

        return true;
    }

    //--------------------------------------------------------------------------------------
    const Skeleton * MeshInstance::getInstanceSkeleton() const
    {
        return m_instanceSkeleton;
    }

    //float3 getBonePosition()

    //--------------------------------------------------------------------------------------
    bool MeshInstance::ShowSkeleton() const
    {
        VG_PROFILE_CPU("ShowSkeleton");

        DebugDraw * dbgDraw = DebugDraw::get();
        //const MeshModel * model = (MeshModel *)getModel(Lod::Lod0);
        //if (nullptr != model)
        {
            const Skeleton * skeleton = getInstanceSkeleton(); // model->m_skeleton;
            if (nullptr != skeleton)
            {
                const auto & nodes = skeleton->m_nodes;
                const auto & boneIndices = skeleton->m_boneIndices;
                for (uint j = 0; j < boneIndices.size(); ++j)
                {
                    int i = boneIndices[j];

                    const MeshImporterNode & node = nodes[i];
                    //if (0xFFFFFFFF != node.parent_index)
                    //    node.node_to_world = mul(nodes[node.parent_index].node_to_world, node.node_to_world);
                    //else
                    //    node.node_to_world = node.node_to_parent;

                    // YUp skeleton displayed as ZUp
                    float4x4 boneMatrix = mul(transpose(node.node_to_world), float4x4::rotation_x(PI / 2.0f));
                             boneMatrix = mul(getWorldMatrix(), boneMatrix);
                    
                    float3 boxSize = float3(0.01f, 0.01f, 0.01f);
                    dbgDraw->AddWireframeBox( -boxSize, boxSize, 0xFF00FF00, boneMatrix);

                    if (0xFFFFFFFF != node.parent_index)
                    {
                        const MeshImporterNode & parentNode = nodes[node.parent_index];
                        float4x4 parentBoneMatrix = mul(transpose(parentNode.node_to_world), float4x4::rotation_x(PI / 2.0f));
                                 parentBoneMatrix = mul(getWorldMatrix(), parentBoneMatrix);

                        dbgDraw->AddLine(boneMatrix[3].xyz, parentBoneMatrix[3].xyz, 0xFF00FF00);
                    }
                }

                return true;
            }
        }

        return false;
    }
}