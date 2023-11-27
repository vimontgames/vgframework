#pragma once

#include "renderer/IMeshInstance.h"

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

    class MeshInstance : public IMeshInstance
    {
    public:
        VG_CLASS_DECL(MeshInstance, IMeshInstance);

        MeshInstance(const core::string & _name, core::IObject * _parent);
        ~MeshInstance();

        void                            SetModel            (core::Lod _lod, core::IModel * _model) final override;

        bool                            HasSkeleton         () const final override;
        bool                            UpdateSkeleton      () final override;
        bool                            ShowSkeleton        () const final override;

        bool                            AddAnimation        (ISkeletalAnimation * _animation) final override;
        bool                            RemoveAnimation     (ISkeletalAnimation * _animation) final override;

        void                            Draw                (const RenderContext & _renderContext, gfx::CommandList * _cmdList) const final override;

        bool                            setInstanceSkeleton (const Skeleton * _skeleton);
        VG_INLINE const Skeleton *      getInstanceSkeleton () const;

        bool                            updateSkeleton      ();

    private:
        Skeleton *                      m_instanceSkeleton = nullptr;

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