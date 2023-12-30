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
}

namespace vg::renderer
{
    class RendererOptions;
    class Skeleton;
    class SkeletalAnimation;
    class MeshModel;

    class MeshInstance : public IMeshInstance
    {
    public:
        VG_CLASS_DECL(MeshInstance, IMeshInstance);

        enum SkinFlags : core::u32
        {
            SkinLOD0 = 0x00000001,
            //SkinLOD1 = 0x00000002,
            //SkinLOD2 = 0x00000004,
            //SkinLOD3 = 0x00000008
        };

        MeshInstance(const core::string & _name, core::IObject * _parent);
        ~MeshInstance();

        void                            SetModel                    (core::Lod _lod, core::IModel * _model) final override;

        bool                            IsSkinned                   () const final override;
        bool                            UpdateSkeleton              () final override;
        bool                            ShowSkeleton                () const final override;

        bool                            AddAnimation                (ISkeletalAnimation * _animation) final override;
        bool                            RemoveAnimation             (ISkeletalAnimation * _animation) final override;

        bool                            SetAnimationTime            (ISkeletalAnimation * _animation, float _time) final override;
        bool                            SetAnimationWeight          (ISkeletalAnimation * _animation, float _weight) final override;

        bool                            GetAABB                     (AABB & _aabb) const final override;
        bool                            Cull                        (const Frustum & _frustum, CullingResult * _cullingResult) final override;

        bool                            OnUpdateRayTracing          (gfx::CommandList * _cmdList, View * _view, core::uint _index) final override;

        void                            Draw                        (const RenderContext & _renderContext, gfx::CommandList * _cmdList) const final override;

        bool                            setInstanceSkeleton         (const Skeleton * _skeleton);
        VG_INLINE const Skeleton *      getInstanceSkeleton         () const;
        const MeshModel *               getMeshModel                (core::Lod _lod) const;

        bool                            updateSkeleton              ();
        void                            setSkinnedMesh              (const gfx::Buffer * _skinnedBuffer, core::uint _skinnedBufferOffset);
        VG_INLINE const gfx::Buffer *   getSkinnedMeshBuffer        () const;
        VG_INLINE const core::uint      getSkinnedMeshBufferOffset  () const;

        VG_INLINE void                  setInstanceBLAS             (gfx::BLAS * _blas);
        VG_INLINE gfx::BLAS *           getInstanceBLAS             () const;

        VG_INLINE bool                  setSkinFlag                 (SkinFlags _flag);
        VG_INLINE bool                  clearSkinFlag               (SkinFlags _flag);

    private:
        core::atomic<core::u32>         m_skinFlags;
        Skeleton *                      m_instanceSkeleton = nullptr;
        const gfx::Buffer *             m_skinnedMeshBuffer = nullptr;
        core::uint                      m_skinnedMeshBufferOffset = 0;
        gfx::BLAS *                     m_instanceBLAS = nullptr;

        struct AnimationState
        {
            float m_time = 0.0f;
            float m_weight = 0.0f;
        };

        struct AnimationBinding
        {
            SkeletalAnimation *         m_animation = nullptr;
            AnimationState              m_animationState;
            core::vector<core::i16>     m_animToSkeletonIndex;
            core::vector<core::i16>     m_skeletonToAnimIndex;
            float                       m_normalizedWeight = 0.0f;
        };
        core::vector<AnimationBinding>  m_animationBindings;
    };
}

#if VG_ENABLE_INLINE
#include "MeshInstance.inl"
#endif