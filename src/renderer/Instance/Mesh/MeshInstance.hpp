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
#include "renderer/Animation/SkeletalAnimation.h"

#include "shaders/system/rootConstants3D.hlsli"

using namespace vg::core;
using namespace vg::gfx;

namespace vg::renderer
{
    VG_REGISTER_OBJECT_CLASS_EX(MeshInstance, "Mesh Instance", core::IClassDesc::Flags::Instance);

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
        VG_SAFE_RELEASE(m_animation);
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

    //--------------------------------------------------------------------------------------
    float4x4 MakeTRS(const float3 & _translation, const quaternion _rotation, const float3 _scale)
    {
        quaternion q = _rotation;

        float xx = q.x * q.x, xy = q.x * q.y, xz = q.x * q.z, xw = q.x * q.w;
        float yy = q.y * q.y, yz = q.y * q.z, yw = q.y * q.w;
        float zz = q.z * q.z, zw = q.z * q.w;
        float sx = 2.0f * _scale.x, sy = 2.0f * _scale.y, sz = 2.0f * _scale.z;

        return float4x4(
            sx * (-yy - zz + 0.5f), sy * (-zw + xy), sz * (+xz + yw), _translation.x,
            sx * (+xy + zw), sy * (-xx - zz + 0.5f), sz * (-xw + yz), _translation.y,
            sx * (-yw + xz), sy * (+xw + yz), sz * (-xx - yy + 0.5f), _translation.z,
            0, 0, 0, 1
            );
    }

    //--------------------------------------------------------------------------------------
    bool MeshInstance::updateSkeleton()
    {
        if (m_animation && m_instanceSkeleton)
        {
            const AnimImporterData & anim = m_animation->m_animData;

            static float time = 0.0f;

            time += 0.005f;

            if (time > anim.time_end - anim.time_begin)
                time = 0;

            float frame_time = (time - anim.time_begin) * anim.framerate;
            uint f0 = min((uint)frame_time + 0, anim.num_frames - 1);
            uint f1 = min((uint)frame_time + 1, anim.num_frames - 1);
            float t = min(frame_time - (float)f0, 1.0f);

            Skeleton & skeleton = *m_instanceSkeleton;
            core::vector<MeshImporterNode> & skeletonNodes = skeleton.m_nodes;

            for (size_t i = 0; i < skeletonNodes.size()-1; i++)
            {
                const AnimNodeData & animNode = anim.animNodes[i];
                
                quaternion rot = animNode.rot.size() > 0 ? lerp(animNode.rot[f0], animNode.rot[f1], t) : animNode.const_rot;
                float3 pos = animNode.pos.size() > 0 ? lerp(animNode.pos[f0], animNode.pos[f1], t) : animNode.const_pos;
                float3 scale = animNode.scale.size() > 0 ? lerp(animNode.scale[f0], animNode.scale[f1], t) : animNode.const_scale;
                
                uint index = i;
                if (i == 0)
                    index = 0;
                else if (i > 1)
                    index = i + 1;

                MeshImporterNode & skeletonNode = skeletonNodes[index];
                skeletonNode.node_to_parent = MakeTRS(pos, rot, scale);
            }

            // update hierarchy
            {
                for (size_t i = 0; i < skeletonNodes.size() - 1; i++)
                {
                    if (i == 1)
                        continue;

                    MeshImporterNode & skeletonNode = skeletonNodes[i];

                    int index = 0;
                    if (i > 1)
                        index = i - 1;

                    if (skeletonNode.parent_index != 0xFFFFFFFF)
                    {
                        const AnimNodeData & animNode = anim.animNodes[index];
                        skeletonNode.node_to_world = mul(skeletonNodes[skeletonNode.parent_index].node_to_world, skeletonNode.node_to_parent);
                    }
                    else
                    {
                        skeletonNode.node_to_world = skeletonNode.node_to_parent;
                    }
                }
                //for (size_t i = 0; i < vs->num_nodes; i++) 
                //{
                //    viewer_node * vn = &vs->nodes[i];
                //
                //    // ufbx stores nodes in order where parent nodes always precede child nodes so we can
                //    // evaluate the transform hierarchy with a flat loop.
                //    if (vn->parent_index >= 0)
                //    {
                //        vn->node_to_world = um_mat_mul(vs->nodes[vn->parent_index].node_to_world, vn->node_to_parent);
                //    }
                //    else {
                //        vn->node_to_world = vn->node_to_parent;
                //    }
                //    vn->geometry_to_world = um_mat_mul(vn->node_to_world, vn->geometry_to_node);
                //    vn->normal_to_world = um_mat_transpose(um_mat_inverse(vn->geometry_to_world));
                //}
            }
       
        }
        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MeshInstance::AddAnimation(ISkeletalAnimation * _animation)
    {
        if (_animation != m_animation)
        {
            VG_SAFE_RELEASE(m_animation);
            VG_SAFE_INCREASE_REFCOUNT(_animation);
            m_animation = (SkeletalAnimation*)_animation;
            
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool MeshInstance::RemoveAnimation(ISkeletalAnimation * _animation)
    {
        if (_animation == m_animation)
        {
            VG_SAFE_RELEASE(m_animation);
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool MeshInstance::ShowSkeleton() const
    {
        ((MeshInstance*)this)->updateSkeleton();

        VG_PROFILE_CPU("ShowSkeleton");

        float4x4 YUpToZUpMatrix = float4x4::rotation_x(PI / 2.0f); // mul(float4x4::rotation_x(PI / 2.0f), float4x4::rotation_y(PI));

        DebugDraw * dbgDraw = DebugDraw::get();
        //const MeshModel * model = (MeshModel *)getModel(Lod::Lod0);
        //if (nullptr != model)
        {
            //const Skeleton * skeleton = model->m_skeleton;
            const Skeleton * skeleton = getInstanceSkeleton(); 
            
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
                    float4x4 boneMatrix = mul(transpose(node.node_to_world), YUpToZUpMatrix);
                             boneMatrix = mul(boneMatrix, getWorldMatrix());
                    
                    float3 boxSize = float3(0.01f, 0.01f, 0.01f);
                    dbgDraw->AddWireframeBox( -boxSize, boxSize, 0xFF00FF00, boneMatrix);

                    if (0xFFFFFFFF != node.parent_index)
                    {
                        const MeshImporterNode & parentNode = nodes[node.parent_index];
                        float4x4 parentBoneMatrix = mul(transpose(parentNode.node_to_world), YUpToZUpMatrix);
                                 parentBoneMatrix = mul(parentBoneMatrix, getWorldMatrix());

                        dbgDraw->AddLine(boneMatrix[3].xyz, parentBoneMatrix[3].xyz, 0xFF00FF00);
                    }
                }

                return true;
            }
        }

        return false;
    }
}