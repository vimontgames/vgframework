#pragma once

#include "renderer/IMeshInstance.h"
#include "gfx/BindlessTable/BindlessTable_consts.h"

namespace vg::core
{
    class IFactory;
    class IClassDesc;
    class IModel;

    enum class Lod : core::u8;
}

namespace vg::gfx
{
    class Buffer;
    class BLAS;
    using BLASVariantKey = core::u64;
}

namespace vg::renderer
{
    class RendererOptions;
    class Skeleton;
    class SkeletalAnimation;
    class MeshModel;

    struct AnimationState
    {
        float m_time = 0.0f;
        float m_weight = 0.0f;
    };

    struct AnimationBinding
    {
        SkeletalAnimation *     m_animation = nullptr;
        core::uint              m_layer = 0;
        BodyPartFlags           m_bodyParts = (BodyPartFlags)0x0;
        AnimationState          m_animationState;
        core::vector<core::i16> m_animToSkeletonIndex;
        core::vector<core::i16> m_skeletonToAnimIndex;
        float                   m_normalizedWeight = 0.0f;
    };

    class MeshInstance final : public IMeshInstance
    {
    public:
        VG_CLASS_DECL(MeshInstance, IMeshInstance);

        MeshInstance(const core::string & _name, core::IObject * _parent);
        ~MeshInstance();

        void                            SetModel                    (core::Lod _lod, core::IModel * _model) final override;

        bool                            IsSkinned                   () const final override;
        bool                            UpdateSkeleton              () final override;
        bool                            DrawSkeleton                (const core::IWorld * _world) const final override;
        const ISkeleton *               GetSkeleton                 () const final override;

        bool                            AddAnimation                (ISkeletalAnimation * _animation) final override;
        bool                            RemoveAnimation             (ISkeletalAnimation * _animation) final override;

        bool                            SetAnimationLayer           (ISkeletalAnimation * _animation, core::uint _layer) final override;
        bool                            SetAnimationBodyParts       (ISkeletalAnimation * _animation, BodyPartFlags _flags) final override;

        bool                            SetAnimationTime            (ISkeletalAnimation * _animation, float _time) final override;
        bool                            SetAnimationWeight          (ISkeletalAnimation * _animation, float _weight) final override;

        core::string                    GetBatchName                (core::uint _batchIndex) const final override;

        bool                            TryGetAABB                  (core::AABB & _aabb) const final override;
        bool                            Cull                        (CullingResult * _cullingResult, View * _view) const final override;
        void                            OnMaterialChanged           (core::uint _index) final override;
        bool                            OnUpdateRayTracing          (gfx::CommandList * _cmdList, View * _view, core::uint _index) final override;

        void                            Draw                        (const RenderContext & _renderContext, gfx::CommandList * _cmdList) const final override;

        core::uint                      GetGPUInstanceDataSize      () const final override;
        core::uint                      FillGPUInstanceData         (const core::u8 * VG_RESTRICT _data) const final override;

        bool                            GetIndexBuffer              (gfx::BindlessBufferHandle & _vb, core::uint & _offset, core::uint & _indexSize) const final override;
        bool                            GetVertexBuffer             (gfx::BindlessBufferHandle & _vb, core::uint & _offset) const final override;
        bool                            GetVertexFormat             (VertexFormat & _vertexFormat) const final override;
        core::uint                      GetBatchCount               () const final override;
        core::uint                      GetBatchOffset              (core::uint _index) const final override;

        bool                            setInstanceSkeleton         (const Skeleton * _skeleton);
        VG_INLINE const Skeleton *      getInstanceSkeleton         () const;
        const MeshModel *               getMeshModel                (core::Lod _lod) const;

        bool                            updateSkeleton              ();
        void                            setSkinnedMesh              (const gfx::Buffer * _skinnedBuffer, core::uint _skinnedBufferOffset);
        VG_INLINE const gfx::Buffer *   getSkinnedMeshBuffer        () const;
        VG_INLINE const core::uint      getSkinnedMeshBufferOffset  () const;

        VG_INLINE void                  setInstanceBLAS             (gfx::BLAS * _BLAS);
        VG_INLINE gfx::BLAS *           getInstanceBLAS             () const;

        gfx::BLASVariantKey             computeBLASVariantKey       () const;
        bool                            updateInstanceBLAS          ();

    private:
        AnimationBinding *              getAnimationBinding         (ISkeletalAnimation * _animation);
        VG_INLINE bool                  isSkinned                   () const;

    private:
        Skeleton *                      m_instanceSkeleton = nullptr;
        const gfx::Buffer *             m_skinnedMeshBuffer = nullptr;
        core::uint                      m_skinnedMeshBufferOffset = -1;
        gfx::BLAS *                     m_instanceBLAS = nullptr;
        core::vector<AnimationBinding>  m_animationBindings;
    };
}

#if VG_ENABLE_INLINE
#include "MeshInstance.inl"
#endif