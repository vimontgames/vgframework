#include "MeshInstance.h"

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
    VG_REGISTER_OBJECT_CLASS_EX(MeshInstance, "Mesh Instance", core::ClassDescFlags::Instance);

    //--------------------------------------------------------------------------------------
    bool MeshInstance::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyObjectPtrEx(MeshInstance, m_instanceSkeleton, "Skeleton", PropertyFlags::Transient);

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
    bool MeshInstance::TryGetAABB(core::AABB & _aabb) const
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
    bool MeshInstance::Cull(CullingResult * _cullingResult, View * _view) const
    {
        const MeshModel * meshModel = getMeshModel(Lod::Lod0);

        bool raytracing = RendererOptions::get()->isRayTracingEnabled();

        if (nullptr != meshModel)
        {
            const core::AABB & aabb = meshModel->getGeometry()->getAABB();

            bool visible = _view->getCameraFrustum().intersects(aabb, getGlobalMatrix()) != FrustumTest::Outside || asBool(ObjectFlags::NoCulling & getObjectFlags());

            if (visible)
            {
                _cullingResult->m_output->add(GraphicInstanceListType::All, this);

                bool hasOpaque = false;
                bool hasAlphaTest = false;
                bool hasAlphaBlend = false;
                bool hasDecal = false;

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

                                case SurfaceType::Decal:
                                    hasDecal = true;
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

                if (hasDecal)
                    _cullingResult->m_output->add(GraphicInstanceListType::Decal, this);

                _cullingResult->m_sharedOutput->addInstance(this);

                if (isSkinned())
                    _cullingResult->m_sharedOutput->addSkinMesh(this);

                const ObjectFlags objectFlags = this->getObjectFlags();

                if (asBool(ObjectFlags::Selected & objectFlags))
                    _cullingResult->m_output->add(GraphicInstanceListType::Outline, this);

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
        if (!isSkinned() && RendererOptions::get()->isRayTracingEnabled())
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
        {
            u32 rayTracingID = getGPUInstanceDataOffset() / GPU_INSTANCE_DATA_ALIGNMENT;
            tlas->addInstance(blas, getGlobalMatrix(), rayTracingID);
        }

        return true;
    }

    //--------------------------------------------------------------------------------------
    uint MeshInstance::GetGPUInstanceDataSize() const
    {
        const uint batchCount = GetBatchCount();
        return alignUp((uint)(sizeof(GPUInstanceData) + batchCount * sizeof(GPUBatchData)), (uint)GPU_INSTANCE_DATA_ALIGNMENT);
    }

    //--------------------------------------------------------------------------------------
    uint MeshInstance::FillGPUInstanceData(const core::u8 * VG_RESTRICT _data) const
    {
        const MeshModel * model = VG_SAFE_STATIC_CAST(MeshModel, getModel(Lod::Lod0));
        const MeshGeometry * geo = nullptr;

        uint batchCount = 1;
        VertexFormat vertexFormat = (VertexFormat)-1;
        
        BindlessBufferHandle ibHandle;
        uint ibOffset, indexSize;

        BindlessBufferHandle vbHandle;
        uint vbOffset;

        const auto & materials = getMaterials();
        const uint materialCount = (uint)materials.size();       
        
        if (nullptr != model)
        {
            geo = model->getGeometry();
            batchCount = (uint)geo->batches().size();
            vertexFormat = geo->getVertexFormat();

            const Buffer * ib = geo->getIndexBuffer();
            ibHandle = ib->getBufferHandle();
            ibOffset = 0;
            indexSize = ib->getBufDesc().elementSize;

            if (isSkinned())
            {
                vbHandle = m_skinnedMeshBuffer->getBufferHandle();
                vbOffset = m_skinnedMeshBufferOffset;
            }
            else
            {
                vbHandle = geo->getVertexBuffer()->getBufferHandle();
                vbOffset = geo->getVertexBufferOffset();
            }
        }

        const size_t instanceDataSize = GetGPUInstanceDataSize();

        GPUInstanceData * VG_RESTRICT instanceData = (GPUInstanceData * VG_RESTRICT)_data;

        instanceData->setMaterialCount(materialCount);
        instanceData->setVertexFormat(vertexFormat);
        instanceData->setInstanceColor(getColor());
        instanceData->setIndexBuffer(ibHandle, indexSize, ibOffset);
        instanceData->setVertexBuffer(vbHandle, vbOffset);

        const auto * renderer = Renderer::get();
        const auto * defaultMaterial = renderer->getDefaultMaterial();
        VG_ASSERT(defaultMaterial);
        const auto defaultMaterialIndex = defaultMaterial->getGPUMaterialDataIndex();
        VG_ASSERT(defaultMaterialIndex == 0);

        for (uint b = 0; b < batchCount; ++b)
        {
            GPUBatchData * VG_RESTRICT batchData = (GPUBatchData * VG_RESTRICT)(_data + sizeof(GPUInstanceData) + b * sizeof(GPUBatchData));

            const MaterialModel * mat = (b < materialCount) ? materials[b] : nullptr;
            GPUMaterialDataIndex matIndex = (nullptr != mat) ? mat->getGPUMaterialDataIndex() : defaultMaterialIndex;
            batchData->setMaterialIndex(matIndex);
                
            uint batchOffset = (nullptr != geo && b < geo->batches().size()) ? geo->batches()[b].offset : 0;
            batchData->setStartIndex(batchOffset);
        }

        return (uint)instanceDataSize;
    }

    //--------------------------------------------------------------------------------------
    bool MeshInstance::GetIndexBuffer(gfx::BindlessBufferHandle & _ib, core::uint & _offset, core::uint & _indexSize) const
    {
        const MeshModel * model = VG_SAFE_STATIC_CAST(MeshModel, getModel(Lod::Lod0));
        VG_ASSERT(model);

        if (nullptr != model)
        {
            const MeshGeometry * geo = model->getGeometry();
            VG_ASSERT(geo);
            const Buffer * ib = geo->getIndexBuffer();
            _ib = ib->getBufferHandle();
            _offset = 0;
            _indexSize = ib->getBufDesc().elementSize;
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool MeshInstance::GetVertexBuffer(gfx::BindlessBufferHandle & _vb, core::uint & _offset) const
    {
        const MeshModel * model = VG_SAFE_STATIC_CAST(MeshModel, getModel(Lod::Lod0));
        VG_ASSERT(model);

        if (nullptr != model)
        {
            const MeshGeometry * geo = model->getGeometry();
            VG_ASSERT(geo);

            if (IsSkinned())
            {
                _vb = m_skinnedMeshBuffer->getBufferHandle();
                _offset = m_skinnedMeshBufferOffset;
            }
            else
            {
                _vb = geo->getVertexBuffer()->getBufferHandle();
                _offset = geo->getVertexBufferOffset();
            }

            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool MeshInstance::GetVertexFormat(VertexFormat & _vertexFormat) const
    {
        const MeshModel * model = VG_SAFE_STATIC_CAST(MeshModel, getModel(Lod::Lod0));
        VG_ASSERT(model);

        if (nullptr != model)
        {
            const MeshGeometry * geo = model->getGeometry();
            VG_ASSERT(geo);
            _vertexFormat = geo->getVertexFormat();
            return true;
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    core::uint MeshInstance::GetBatchCount() const
    {
        const MeshModel * model = VG_SAFE_STATIC_CAST(MeshModel, getModel(Lod::Lod0));
        VG_ASSERT(model);

        if (nullptr != model)
        {
            const MeshGeometry * geo = model->getGeometry();
            VG_ASSERT(geo);
            return (uint)geo->batches().size();
        }
        
        return 1;
    }

    //--------------------------------------------------------------------------------------
    core::uint MeshInstance::GetBatchOffset(core::uint _index) const
    {
        const MeshModel * model = VG_SAFE_STATIC_CAST(MeshModel, getModel(Lod::Lod0));
        VG_ASSERT(model);

        if (nullptr != model)
        {
            const MeshGeometry * geo = model->getGeometry();
            VG_ASSERT(geo);
            if (_index < geo->batches().size())
                return geo->batches()[_index].offset;
        }

        return 0;
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

            BindlessBufferHandle vbHandle;
            uint vbOffset;
            VG_VERIFY(GetVertexBuffer(vbHandle, vbOffset));
                        
            root3D.setVertexBufferHandle(vbHandle, vbOffset);

            u16 flags = 0;
            root3D.setFlags(flags);

            VertexFormat vertexFormat;
            VG_VERIFY(GetVertexFormat(vertexFormat));
            root3D.setVertexFormat(vertexFormat);

            auto pickingID = GetPickingID();
            root3D.setPickingID(pickingID);

            _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleList);
            const BitMask & batchMask = getBatchMask();

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

                    if (_renderContext.m_surfaceType != surfaceType && !_renderContext.m_wireframe && !_renderContext.m_outline)
                        continue;

                    switch (_renderContext.m_shaderPass)
                    {
                        default:
                            VG_ASSERT_ENUM_NOT_IMPLEMENTED(_renderContext.m_shaderPass);
                            continue;

                        case ShaderPass::ZOnly:
                        case ShaderPass::Forward:
                        case ShaderPass::Deferred:
                        case ShaderPass::Outline:
                            material->Setup(_renderContext, _cmdList, &root3D, i);
                            break;

                        case ShaderPass::Transparent:
                            material->Setup(_renderContext, _cmdList, &root3D, i);
                            break;
                    }

                    _cmdList->setGraphicRootConstants(0, (u32 *)&root3D, RootConstants3DCount);
                    _cmdList->drawIndexed(batch.count, batch.offset);
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

        if (_skeleton == nullptr)
        {
            VG_SAFE_RELEASE(m_instanceSkeleton);
        }
        else
        {
            if (nullptr == m_instanceSkeleton)
                m_instanceSkeleton = new Skeleton("InstanceSkeleton", this);

            // TODO: copy ctor?
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

            // Compute T-Pose weight for each layer
            float sum[MaxAnimationLayerCount];

            for (uint iLayer = 0; iLayer < MaxAnimationLayerCount; ++iLayer)
            {
                sum[iLayer] = 0.0f;

                for (uint j = 0; j < m_animationBindings.size(); ++j)
                {
                    if (m_animationBindings[j].m_layer == iLayer)
                        sum[iLayer] += m_animationBindings[j].m_animationState.m_weight;
                }
            }

            float tPoseWeight[MaxAnimationLayerCount];
            for (uint iLayer = 0; iLayer < MaxAnimationLayerCount; ++iLayer)
            {
                if (sum[iLayer] > 1.0f)
                    tPoseWeight[iLayer] = 0.0f;
                else
                    tPoseWeight[iLayer] = 1.0f - sum[iLayer];
            }

            for (uint iLayer = 0; iLayer < MaxAnimationLayerCount; ++iLayer)
            {
                for (uint j = 0; j < m_animationBindings.size(); ++j)
                {
                    if (sum[iLayer] > 1.0f)
                        m_animationBindings[j].m_normalizedWeight = m_animationBindings[j].m_animationState.m_weight / sum[iLayer];
                    else
                        m_animationBindings[j].m_normalizedWeight = m_animationBindings[j].m_animationState.m_weight;
                }
            }

            bool forceTPose = false;
            float sumLayers = 0.0f;
            for (uint iLayer = 0; iLayer < MaxAnimationLayerCount; ++iLayer)
                sumLayers += sum[iLayer];
            if (sumLayers <= 0.0f)
                forceTPose = true;

            // Update animations and blend
            for (size_t i = 0; i < skeletonNodes.size(); i++)
            {
                MeshImporterNode & skeletonNode = skeletonNodes[i];

                float3 translation = (float3)0.0f;
                quaternion rotation = quaternion::identity();
                float3 scale = (float3)1.0f;

                float accumulateLayerWeights = 0;

                for (uint iLayer = 0; iLayer < MaxAnimationLayerCount; ++iLayer)
                {
                    if (sum[iLayer] <= 0.0f && !forceTPose)
                        continue;

                    //float3 lTranslation = skeletonNode.pos * tPoseWeight[iLayer];
                    //quaternion lRotation = skeletonNode.rot * tPoseWeight[iLayer];
                    //float3 lScale = skeletonNode.scale * tPoseWeight[iLayer];

                    float3 lTranslation = skeletonNode.pos;
                    quaternion lRotation = skeletonNode.rot;
                    float3 lScale = skeletonNode.scale;

                    bool applyLayer = forceTPose && iLayer == 0;

                    float accumulatedAnimWeights = tPoseWeight[iLayer];

                    for (uint j = 0; j < m_animationBindings.size(); ++j)
                    {
                        AnimationBinding & animationBinding = m_animationBindings[j];

                        if (animationBinding.m_layer != iLayer)
                            continue;

                        if (!asBool(skeletonNode.flags & animationBinding.m_bodyParts))
                            continue;

                        const SkeletalAnimation * animation = animationBinding.m_animation;
                        const AnimImporterData & anim = animation->getAnimationData();

                        const float time = animationBinding.m_animationState.m_time;
                        const float frame_time = (time - anim.time_begin) * anim.framerate;

                        const uint f0 = min((uint)frame_time + 0, anim.num_frames - 1);
                        const uint f1 = min((uint)frame_time + 1, anim.num_frames - 1);
                        const float t = min(frame_time - (float)f0, 1.0f);

                        // index in animation
                        const int index = animationBinding.m_skeletonToAnimIndex[i];

                        if (-1 != index)
                        {
                            if (animationBinding.m_normalizedWeight > 0.0f)
                            {
                                const AnimNodeData & animNode = anim.animNodes[index];

                                const float3 aTranslation = animNode.pos.size() > 0 ? lerp(animNode.pos[f0], animNode.pos[f1], t) : animNode.const_pos;
                                const quaternion aRotation = animNode.rot.size() > 0 ? slerpShortestPath(animNode.rot[f0], animNode.rot[f1], t) : animNode.const_rot;
                                const float3 aScale = animNode.scale.size() > 0 ? lerp(animNode.scale[f0], animNode.scale[f1], t) : animNode.const_scale;

                                const float weight = animationBinding.m_normalizedWeight;
                                accumulatedAnimWeights += weight;
                                const float normalizedAnimWeight = weight / accumulatedAnimWeights;
                                lTranslation = lerp(lTranslation, aTranslation, normalizedAnimWeight);
                                lRotation = slerpShortestPath(lRotation, aRotation, normalizedAnimWeight);
                                lScale = lerp (lScale, aScale, normalizedAnimWeight);

                                applyLayer = true;
                            }
                        }
                    }

                    // Blend & mask
                    if (applyLayer)
                    {
                        const float layerWeight = forceTPose ? 1.0f : saturate(sum[iLayer]);
                        const float normalizedLayerWeight = (accumulateLayerWeights == 0.0f) ? 1 : layerWeight / accumulateLayerWeights;
                        accumulateLayerWeights += layerWeight;

                        translation = lerp(translation, lTranslation, normalizedLayerWeight);
                        rotation = slerpShortestPath(rotation, lRotation, normalizedLayerWeight);
                        scale = lerp(scale, lScale, normalizedLayerWeight);
                    }
                }

                // Update skeleton node
                skeletonNode.node_to_parent = TRSToFloat4x4(translation, rotation, scale);

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

        const Skeleton * meshSkeleton = meshModel->getSkeleton();
        VG_ASSERT(meshSkeleton);

        if (!meshSkeleton)
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
            binding = &m_animationBindings.emplace_back();
            binding->m_animation = (SkeletalAnimation*)_animation;
        }    

        // Rebind anim nodes indices to skeleton nodes indices
        const vector<AnimNodeData> & animNodes = binding->m_animation->getAnimationData().animNodes;
        binding->m_animToSkeletonIndex.reserve(animNodes.size());

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
            VG_ASSERT(-1 != index, "Could not find node \"%s\" (#%u) from animation \"%s\" in skin \"%s\"", animNode.name.c_str(), i, binding->m_animation->GetName().c_str(), meshModel->GetName().c_str());
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
    AnimationBinding * MeshInstance::getAnimationBinding(ISkeletalAnimation * _animation)
    {
        for (uint i = 0; i < m_animationBindings.size(); ++i)
        {
            AnimationBinding & animationBinding = m_animationBindings[i];
            if (_animation == animationBinding.m_animation)
                return &animationBinding;
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    bool MeshInstance::SetAnimationLayer(ISkeletalAnimation * _animation, core::uint _layer)
    {
        VG_ASSERT(_layer < MaxAnimationLayerCount);
        _layer = min(_layer, MaxAnimationLayerCount-1);

        if (auto * animationBinding = getAnimationBinding(_animation))
        {
            animationBinding->m_layer = _layer;
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool MeshInstance::SetAnimationBodyParts(ISkeletalAnimation * _animation, BodyPartFlags _flags)
    {
        if (auto * animationBinding = getAnimationBinding(_animation))
        {
            animationBinding->m_bodyParts = _flags;
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool MeshInstance::SetAnimationTime(ISkeletalAnimation * _animation, float _time)
    {
        if (auto * animationBinding = getAnimationBinding(_animation))
        {
            animationBinding->m_animationState.m_time = _time;
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool MeshInstance::SetAnimationWeight(ISkeletalAnimation * _animation, float _weight)
    {
        if (auto * animationBinding = getAnimationBinding(_animation))
        {
            animationBinding->m_animationState.m_weight = _weight;
            return true;
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
                    return batches[_batchIndex].GetName();
            }
        }
        return "";
    }

    //--------------------------------------------------------------------------------------
    bool MeshInstance::IsSkinned() const
    {
        return isSkinned();
    }

    //--------------------------------------------------------------------------------------
    bool MeshInstance::UpdateSkeleton()
    {
        return updateSkeleton();
    }

    //--------------------------------------------------------------------------------------
    const ISkeleton * MeshInstance::GetSkeleton() const
    {
        return m_instanceSkeleton;
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

                float4x4 nodeMatrix = transpose(node.node_to_world);

                // YUp skeleton displayed as ZUp
                float4x4 boneMatrix = mul(nodeMatrix, matrix) ;

                u32 color = 0x0;
                const bool upper = asBool(node.flags & renderer::BodyPartFlags::UpperBody);
                const bool lower = asBool(node.flags & renderer::BodyPartFlags::LowerBody);

                if (upper || lower)
                {
                    if (upper)
                        color |= 0xFF0000FF;

                    if (lower)
                        color |= 0xFF00FF00;
                }
                else
                {
                    color = 0xFF7F7F7F;
                }

                float3 boxSize = float3(0.01f, 0.01f, 0.01f);

                if (skeleton->IsNodeSelected(index))
                {
                    color |= 0xFFCFCFCF;
                    boxSize *= 1.5f;
                }

                dbgDraw->AddWireframeBox(_world, -boxSize, boxSize, color, boneMatrix);

                if (-1 != node.parent_index)
                {
                    const MeshImporterNode & parentNode = nodes[node.parent_index];
                    float4x4 parentNodeMatrix = transpose(parentNode.node_to_world);
                    float4x4 parentBoneMatrix = mul(parentNodeMatrix, matrix) ;
                    dbgDraw->AddLine(_world, boneMatrix[3].xyz, parentBoneMatrix[3].xyz, color);
                }
            }

            return true;
        }

        return false;
    }
}