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

namespace vg::renderer
{
    class DisplayOptions;
    class Skeleton;
    class SkeletalAnimation;
    class MeshModel;

    class MeshInstance : public IMeshInstance
    {
    public:
        VG_CLASS_DECL(MeshInstance, IMeshInstance);

        enum RuntimeFlags : core::u32
        {
            SkinLOD0 = 0x00000001,
            //SkinLOD1 = 0x00000002,
            //SkinLOD2 = 0x00000004,
            //SkinLOD3 = 0x00000008
        };

        MeshInstance(const core::string & _name, core::IObject * _parent);
        ~MeshInstance();

        void                            SetModel            (core::Lod _lod, core::IModel * _model) final override;

        bool                            IsSkinned           () const final override;
        bool                            UpdateSkeleton      () final override;
        bool                            ShowSkeleton        () const final override;

        bool                            AddAnimation        (ISkeletalAnimation * _animation) final override;
        bool                            RemoveAnimation     (ISkeletalAnimation * _animation) final override;

        void                            Draw                (const RenderContext & _renderContext, gfx::CommandList * _cmdList) const final override;

        bool                            setInstanceSkeleton (const Skeleton * _skeleton);
        VG_INLINE const Skeleton *      getInstanceSkeleton () const;
        const MeshModel *               getMeshModel        (core::Lod _lod) const;

        bool                            updateSkeleton      ();
        void                            setSkinnedMesh      (const gfx::BindlessBufferHandle & _skinnedBufferHandle, core::uint _skinnedBufferOffset);

        VG_INLINE bool                  setRuntimeFlag      (RuntimeFlags _flag);
        VG_INLINE bool                  clearRuntimeFlag    (RuntimeFlags _flag);

    private:
        core::atomic<core::u32>         m_runtimeFlags;
        Skeleton *                      m_instanceSkeleton = nullptr;
        gfx::BindlessBufferHandle       m_skinnedMeshBufferHandle;
        core::uint                      m_skinnedMeshBufferOffset = 0;

        struct AnimationBinding
        {
            SkeletalAnimation *         m_animation = nullptr;
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