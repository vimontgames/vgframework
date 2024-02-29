#include "renderer/Precomp.h"
#include "MeshInstance.h"

#include "core/Object/AutoRegisterClass.h"

#include "gfx/CommandList/CommandList.h"
#include "gfx/Resource/Buffer.h"
#include "gfx/Resource/Texture.h"
#include "gfx/Raytracing/TLAS.h"

#include "renderer/Renderer.h"
#include "renderer/Options/RendererOptions.h"
#include "renderer/Model/Mesh/MeshModel.h"
#include "renderer/Model/Material/MaterialModel.h"
#include "renderer/Geometry/Mesh/MeshGeometry.h"
#include "renderer/Animation/Skeleton.h"
#include "renderer/RenderPass/RenderContext.h"
#include "renderer/DebugDraw/DebugDraw.h"
#include "renderer/Animation/SkeletalAnimation.h"
#include "renderer/View/View.h"
#include "renderer/RayTracing/RayTracingManager.h"

#if !VG_ENABLE_INLINE
#include "MeshInstance.inl"
#endif

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
        auto * rtManager = RayTracingManager::get(false);
        if (rtManager)
            rtManager->addMeshInstance(this);
    }

    //--------------------------------------------------------------------------------------
    MeshInstance::~MeshInstance()
    {
        VG_SAFE_RELEASE(m_instanceBLAS);
        VG_SAFE_RELEASE(m_instanceSkeleton);

        for (uint i = 0; i < m_animationBindings.size(); ++i)
            VG_SAFE_RELEASE(m_animationBindings[i].m_animation);

        auto * rtManager = RayTracingManager::get(false);
        if (rtManager)
            rtManager->removeMeshInstance(this);
    }

    //--------------------------------------------------------------------------------------
    bool MeshInstance::GetAABB(AABB & _aabb) const
    {
        if (auto * model = getMeshModel(Lod::Lod0))
        {
            const MeshGeometry * geo = model->getGeometry();
            _aabb = geo->getAABB();
            return true;
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool MeshInstance::Cull(CullingResult * _cullingResult, View * _view)
    {
        const MeshModel * meshModel = getMeshModel(Lod::Lod0);

        bool raytracing = RendererOptions::get()->isRayTracingEnabled();

        if (nullptr != meshModel)
        {
            const AABB & aabb = meshModel->getGeometry()->getAABB();

            bool visible = _view->getCameraFrustum().intersects(aabb, getGlobalMatrix()) != FrustumTest::Outside || asBool(IInstance::RuntimeFlags::NoCulling & getRuntimeFlags());

            if (visible)
            {
                _cullingResult->m_output->add(GraphicInstanceListType::All, this);

                bool hasOpaque = false;
                bool hasAlphaTest = false;
                bool hasAlphaBlend = false;

                auto materials = getMaterials();
                if (materials.size() > 0)
                {
                    for (auto * material : materials)
                    {
                        if (material)
                        {
                            auto surf = material->getSurfaceType();

                            switch (surf)
                            {
                                case SurfaceType::Opaque:
                                    hasOpaque = true;
                                    break;

                                case SurfaceType::AlphaTest:
                                    hasAlphaTest = true;
                                    break;

                                case SurfaceType::AlphaBlend:
                                    hasAlphaBlend = true;
                                    break;
                            }
                        }
                        else
                        {
                            hasOpaque = true;
                            break;
                        }
                    }
                }
                else
                {
                    hasOpaque = true;
                }

                if (hasOpaque)
                    _cullingResult->m_output->add(GraphicInstanceListType::Opaque, this);

                if (hasAlphaTest)
                    _cullingResult->m_output->add(GraphicInstanceListType::AlphaTest, this);

                if (hasAlphaBlend)
                    _cullingResult->m_output->add(GraphicInstanceListType::Transparent, this);

                if (setAtomicFlags(GraphicInstance::AtomicFlags::Instance))
                    _cullingResult->m_sharedOutput->m_instances.push_back_atomic(this);

                if (IsSkinned())
                {
                    if (setAtomicFlags(GraphicInstance::AtomicFlags::SkinLOD0))
                        _cullingResult->m_sharedOutput->m_skins.push_back_atomic(this);
                }

                return true;
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    gfx::BLASVariantKey MeshInstance::computeBLASVariantKey() const
    {
        const auto & materials = getMaterials();
        VG_ASSERT(materials.size() <= 64);
        
        gfx::BLASVariantKey key = 0;

        MeshModel * meshModel = (MeshModel*)getModel(Lod::Lod0);
        if (meshModel)
        {
            const auto & batches = meshModel->getGeometry()->batches();
            VG_ASSERT(batches.size() <= 64);

            uint bit = 1;
            for (uint i = 0; i < batches.size(); ++i)
            {
                if (i < materials.size())
                {
                    if (const auto * mat = materials[i])
                    {
                        if (gfx::SurfaceType::Opaque != mat->getSurfaceType())
                            key |= bit;
                    }                   
                }

                bit <<= 1;
            }
        }

        return key;
    }    

    //--------------------------------------------------------------------------------------
    bool MeshInstance::updateInstanceBLAS()
    {
        if (!IsSkinned() && RendererOptions::get()->isRayTracingEnabled())
        {
            auto * blas = getInstanceBLAS();

            bool update = false;

            if (nullptr == blas)
            {
                update = true;
            }
            else
            {
                auto key = computeBLASVariantKey();
                if (blas->getKey() != key)
                    update = true;
            }

            if (update)
            {
                RayTracingManager::get()->updateMeshInstance(this);
                return true;
            }
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    void MeshInstance::OnMaterialChanged(core::uint _index)
    {
        if (Model * model = getModel(Lod::Lod0))
            updateInstanceBLAS();
    }

    //--------------------------------------------------------------------------------------
    bool MeshInstance::OnUpdateRayTracing(gfx::CommandList * _cmdList, View * _view, core::uint _index)
    {
        auto * tlas = _view->getTLAS();

        if (const auto * blas = getInstanceBLAS())
            tlas->addInstance(blas, getGlobalMatrix(), _index);

        return true;
    }

    //--------------------------------------------------------------------------------------
    void MeshInstance::Draw(const RenderContext & _renderContext, gfx::CommandList * _cmdList) const
    {
        auto * renderer = Renderer::get();

        const MeshModel * model = (MeshModel *)getModel(Lod::Lod0);
        if (nullptr != model)
        {
            const MeshGeometry * geo = model->getGeometry();

            const float4x4 world = getGlobalMatrix();

            _cmdList->setIndexBuffer(geo->getIndexBuffer());

            RootConstants3D root3D;
            root3D.setWorldMatrix(transpose(world));

            root3D.setGPUInstanceDataOffset(getGPUInstanceDataOffset());

            if (IsSkinned())
                root3D.setBufferHandle(m_skinnedMeshBuffer->getBufferHandle(), m_skinnedMeshBufferOffset);
            else
                root3D.setBufferHandle(geo->getVertexBuffer()->getBufferHandle(), geo->getVertexBufferOffset());
            
            u16 flags = 0;
            root3D.setFlags(flags);

            VertexFormat vertexFormat = geo->getVertexFormat();
            root3D.setVertexFormat(vertexFormat);

            auto pickingID = GetPickingID();
            root3D.setPickingID(pickingID);

            _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleList);
            const BitMask batchMask = getBatchMask();

            const auto & batches = geo->batches();
            for (uint i = 0; i < batches.size(); ++i)
            {
                const auto & batch = batches[i];
                bool draw = false;

                if (batchMask.getBitValue(i))
                {
                    // Setup material 
                    const MaterialModel * material = getMaterial(i);
                    if (nullptr == material)
                        material = renderer->getDefaultMaterial();

                    auto surfaceType = material->getSurfaceType();

                    switch (_renderContext.m_shaderPass)
                    {
                    default:
                        VG_ASSERT_ENUM_NOT_IMPLEMENTED(_renderContext.m_shaderPass);
                        break;

                    case ShaderPass::ZOnly:
                    case ShaderPass::Forward:
                    case ShaderPass::Deferred:
                    {
                        // Do not render transparent material in opaque passes
                        if (SurfaceType::AlphaBlend != surfaceType)
                        {
                            if (_renderContext.m_alphatest == (SurfaceType::AlphaTest == surfaceType))
                            {
                                material->Setup(_renderContext, _cmdList, &root3D, i);
                                draw = true;
                            }
                        }
                    }
                    break;

                    case ShaderPass::Transparent:
                    {
                        if (SurfaceType::AlphaBlend == surfaceType)
                        {
                            material->Setup(_renderContext, _cmdList, &root3D, i);
                            draw = true;
                        }
                    }
                    break;
                    }

                    if (draw)
                    {
                        _cmdList->setGraphicRootConstants(0, (u32 *)&root3D, RootConstants3DCount);
                        _cmdList->drawIndexed(batch.count, batch.offset);
                    }
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void MeshInstance::SetModel(Lod _lod, IModel * _model)
    {
        MeshModel * current = (MeshModel*)getModel(_lod);
        MeshModel * meshModel = (MeshModel *)_model;
        VG_ASSERT(nullptr == _model || nullptr != dynamic_cast<MeshModel *>(_model));

        if (meshModel != current)
            setInstanceBLAS(nullptr);

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

        if (nullptr != _model)
            updateInstanceBLAS();
    }

    //--------------------------------------------------------------------------------------
    const MeshModel * MeshInstance::getMeshModel(core::Lod _lod) const
    {
        return (MeshModel *)getModel(_lod);
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

            m_instanceSkeleton->setNodes(_skeleton->getNodes());
            m_instanceSkeleton->setBoneIndices(_skeleton->getBoneIndices());
            m_instanceSkeleton->setBoneMatrices(_skeleton->getBoneMatrices());                
        }

        return true;
    }

    //--------------------------------------------------------------------------------------
    void MeshInstance::setSkinnedMesh(const gfx::Buffer * _skinnedBuffer, core::uint _skinnedBufferOffset)
    {
        m_skinnedMeshBuffer = _skinnedBuffer;
        m_skinnedMeshBufferOffset = _skinnedBufferOffset;
    }

    //--------------------------------------------------------------------------------------
    bool MeshInstance::updateSkeleton()
    {
        VG_PROFILE_CPU("Skeleton Update");

        bool skeletonUpdate = false;

        if (nullptr != m_instanceSkeleton)
        {
            Skeleton & skeleton = *m_instanceSkeleton;
            core::vector<MeshImporterNode> & skeletonNodes = skeleton.getNodes();

            // normalize weights
            float weightSum = 0.0f;
            for (uint j = 0; j < m_animationBindings.size(); ++j)
            {
                weightSum += m_animationBindings[j].m_animationState.m_weight;
            }
            float tPoseWeight;

            if (weightSum > 1.0f)
            {
                tPoseWeight = 0.0f;
                const float invWeightSum = 1.0f / weightSum;
                for (uint j = 0; j < m_animationBindings.size(); ++j)
                    m_animationBindings[j].m_normalizedWeight = m_animationBindings[j].m_animationState.m_weight * invWeightSum;
            }
            else
            {
                tPoseWeight = 1.0f - weightSum;
                for (uint j = 0; j < m_animationBindings.size(); ++j)
                    m_animationBindings[j].m_normalizedWeight = m_animationBindings[j].m_animationState.m_weight;
            }

            // Update animations and blend
            for (size_t i = 0; i < skeletonNodes.size(); i++)
            {
                MeshImporterNode & skeletonNode = skeletonNodes[i];

                quaternion rot = skeletonNode.rot * tPoseWeight;
                float3 pos = skeletonNode.pos * tPoseWeight;
                float3 scale = skeletonNode.scale * tPoseWeight;

                for (uint j = 0; j < m_animationBindings.size(); ++j)
                {
                    AnimationBinding & animationBinding = m_animationBindings[j];
                    const SkeletalAnimation * animation = animationBinding.m_animation;
                    const AnimImporterData & anim = animation->getAnimationData();

                    const float time = animationBinding.m_animationState.m_time;

                    float frame_time = (time - anim.time_begin) * anim.framerate;
                    uint f0 = min((uint)frame_time + 0, anim.num_frames - 1);
                    uint f1 = min((uint)frame_time + 1, anim.num_frames - 1);
                    float t = min(frame_time - (float)f0, 1.0f);

                    // index in animation
                    const int index = animationBinding.m_skeletonToAnimIndex[i];

                    if (-1 != index)
                    {
                        const AnimNodeData & animNode = anim.animNodes[index];

                        const quaternion animRot = animNode.rot.size() > 0 ? lerp(animNode.rot[f0], animNode.rot[f1], t) : animNode.const_rot;
                        const float3 animPos = animNode.pos.size() > 0 ? lerp(animNode.pos[f0], animNode.pos[f1], t) : animNode.const_pos;
                        const float3 animScale = animNode.scale.size() > 0 ? lerp(animNode.scale[f0], animNode.scale[f1], t) : animNode.const_scale;

                        rot += animRot * animationBinding.m_normalizedWeight;
                        pos += animPos * animationBinding.m_normalizedWeight;
                        scale += animScale * animationBinding.m_normalizedWeight;
                    }
                }

                // Update skeleton node
                skeletonNode.node_to_parent = TRS(pos, rot, scale);

                if (-1 != skeletonNode.parent_index)
                    skeletonNode.node_to_world = mul(skeletonNodes[skeletonNode.parent_index].node_to_world, skeletonNode.node_to_parent);
                else
                    skeletonNode.node_to_world = mul(skeletonNode.node_to_parent, float4x4::rotation_x(-PI / 2.0f));

                skeletonNode.geometry_to_world = mul(skeletonNode.node_to_world, skeletonNode.geometry_to_node);
                skeletonNode.normal_to_world = transpose(inverse(skeletonNode.geometry_to_world));
            }
        }

        return skeletonUpdate;
    }

    //--------------------------------------------------------------------------------------
    bool MeshInstance::AddAnimation(ISkeletalAnimation * _animation)
    {
        const MeshModel * meshModel = (const MeshModel *)getModel(Lod::Lod0);
        if (meshModel == nullptr)
            return false;

        AnimationBinding * binding = nullptr;
        for (uint i = 0; i < m_animationBindings.size(); ++i)
        {
            AnimationBinding & animationBinding = m_animationBindings[i];
            if (animationBinding.m_animation == _animation)
            {
                binding = &animationBinding;
                return true; // No need rebind an animation already binded to current model
            }
        }

        VG_SAFE_INCREASE_REFCOUNT(_animation);

        if (nullptr == binding)
        {
            binding = &m_animationBindings.push_empty();
            binding->m_animation = (SkeletalAnimation*)_animation;
        }    

        // Rebind anim nodes indices to skeleton nodes indices
        const vector<AnimNodeData> & animNodes = binding->m_animation->getAnimationData().animNodes;
        binding->m_animToSkeletonIndex.reserve(animNodes.size());

        const Skeleton * meshSkeleton = meshModel->getSkeleton();
        const vector<MeshImporterNode> & skeletonNodes = meshSkeleton->getNodes();
        
        for (uint i = 0; i < animNodes.size(); ++i)
        {
            const AnimNodeData & animNode = animNodes[i];

            // TODO : use hash computed from node name during import to speed up the process
            int index = -1;
            for (uint j = 0; j < skeletonNodes.size(); ++j)
            {
                const MeshImporterNode & skeletonNode = skeletonNodes[j];
                if (skeletonNode.name == animNode.name)
                {
                    index = j;
                    break;
                }
            }
            VG_ASSERT(-1 != index, "Could not find node \"%s\" (#%u) from animation \"%s\" in skin \"%s\"", animNode.name.c_str(), i, binding->m_animation->getName().c_str(), meshModel->getName().c_str());
            binding->m_animToSkeletonIndex.push_back(index);
        }

        for (uint i = 0; i < skeletonNodes.size(); ++i)
        {
            const MeshImporterNode & skeletonNode = skeletonNodes[i];

            int index = -1;
            for (uint j = 0; j < binding->m_animToSkeletonIndex.size(); ++j)
            {
                if (binding->m_animToSkeletonIndex[j] == i)
                {
                    index = j;
                    break;
                }
            }
            //VG_ASSERT(-1 != index, "Could not find mapping for skin node %u \"%s\" in animation \"%s\"", i, skeletonNode.name.c_str(), _animation->getName().c_str());
            binding->m_skeletonToAnimIndex.push_back(index);
        }

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MeshInstance::RemoveAnimation(ISkeletalAnimation * _animation)
    {
        for (uint i = 0; i < m_animationBindings.size(); ++i)
        {
            AnimationBinding & animationBinding = m_animationBindings[i];
            if (animationBinding.m_animation == _animation)
            {
                VG_SAFE_RELEASE(animationBinding.m_animation);
                m_animationBindings.erase(m_animationBindings.begin()+i);
                return true;
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool MeshInstance::SetAnimationTime(ISkeletalAnimation * _animation, float _time)
    {
        for (uint i = 0; i < m_animationBindings.size(); ++i)
        {
            AnimationBinding & animationBinding = m_animationBindings[i];
            if (_animation == animationBinding.m_animation)
            {
                animationBinding.m_animationState.m_time = _time;
                return true;
            }
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool MeshInstance::SetAnimationWeight(ISkeletalAnimation * _animation, float _weight)
    {
        for (uint i = 0; i < m_animationBindings.size(); ++i)
        {
            AnimationBinding & animationBinding = m_animationBindings[i];
            if (_animation == animationBinding.m_animation)
            {
                animationBinding.m_animationState.m_weight = _weight;
                return true;
            }
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    core::string MeshInstance::GetBatchName(core::uint _batchIndex) const
    {
        if (const MeshModel * model = VG_SAFE_STATIC_CAST(MeshModel, getModel(Lod::Lod0)))
        {
            if (auto * geo = model->getGeometry())
            {
                const auto & batches = geo->batches();
                if (_batchIndex < batches.size())
                    return batches[_batchIndex].getName();
            }
        }
        return "";
    }

    //--------------------------------------------------------------------------------------
    bool MeshInstance::IsSkinned() const
    {
        return nullptr != getInstanceSkeleton();
    }

    //--------------------------------------------------------------------------------------
    bool MeshInstance::UpdateSkeleton()
    {
        return updateSkeleton();
    }

    //--------------------------------------------------------------------------------------
    bool MeshInstance::DrawSkeleton(const core::IWorld * _world) const
    {
        VG_PROFILE_CPU("Skeleton Draw");

        DebugDraw * dbgDraw = DebugDraw::get();

        const Skeleton * skeleton = getInstanceSkeleton(); 
            
        if (nullptr != skeleton)
        {
            const auto & nodes = skeleton->getNodes();
            const auto & boneIndices = skeleton->getBoneIndices();

            for (uint j = 0; j < boneIndices.size(); ++j)
            {
                const int index = boneIndices[j];

                const MeshImporterNode & node = nodes[index];

                const float4x4 matrix = getGlobalMatrix();

                // YUp skeleton displayed as ZUp
                float4x4 boneMatrix = mul(transpose(node.node_to_world), matrix);

                float3 boxSize = float3(0.01f, 0.01f, 0.01f);
                dbgDraw->AddWireframeBox(_world, -boxSize, boxSize, 0xFF00FF00, boneMatrix);

                if (-1 != node.parent_index)
                {
                    const MeshImporterNode & parentNode = nodes[node.parent_index];
                    float4x4 parentBoneMatrix = mul(transpose(parentNode.node_to_world), matrix);
                    dbgDraw->AddLine(_world, boneMatrix[3].xyz, parentBoneMatrix[3].xyz, 0xFF00FF00);
                }
            }

            return true;
        }

        return false;
    }
}