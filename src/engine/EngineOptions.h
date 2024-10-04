#pragma once
#include "core/Singleton/Singleton.h"
#include "engine/IEngineOptions.h"

namespace vg::physics
{
    enum class ShapeType : core::u8;
    enum class ShapeTypeFlags : core::u32;
}

namespace vg::engine
{
    vg_enum_class(AnimationOptionFlags, core::u32,
        Jobs        = 0x00000001,
        Skeleton    = 0x00000002
    );

    class EngineOptions final : public IEngineOptions, public core::Singleton<EngineOptions>
    {
    public:
        VG_CLASS_DECL(EngineOptions, IEngineOptions);

        EngineOptions(const core::string & _name, core::IObject * _parent = nullptr);

        const core::string &    GetProjectPath      () const final override;
        const core::string &    GetStartWorld       () const final override;

        float                   TryGetTimeScale     (float & _timescale) const final override;
        bool                    TryGetFixedDT       (float & _fixedDT) const final override;
        bool                    TryGetMaxDT         (float & _maxDT) const final override;

        void                    OnPropertyChanged   (IObject * _object, const core::IProperty & _prop, bool _notifyParent) final override;

        VG_INLINE bool          useAnimationJobs    () const { return (core::u32)AnimationOptionFlags::Jobs & (core::u32)m_animationOptionFlags; }
        VG_INLINE bool          isShowSkeleton      () const { return (core::u32)AnimationOptionFlags::Skeleton & (core::u32)m_animationOptionFlags; }

        VG_INLINE bool          isBodyVisible       (physics::ShapeType _shape) const { return m_showRigidBodies && ((1 << (core::u32)_shape) & (core::u32)m_showRigidBodiesMask); }
        VG_INLINE bool          mergeStaticBodies   () const { return m_mergeStaticBodies; }

    private:
        // Project
        core::string            m_gamePath;
        core::string            m_startWorld;

        // Physics
        bool                    m_mergeStaticBodies = false;
        bool                    m_showRigidBodies = false;
        physics::ShapeTypeFlags m_showRigidBodiesMask = (physics::ShapeTypeFlags)-1;

        // Time
        bool                    m_useTimeScale = false;
        float                   m_timeScale = 1.0f;
        bool                    m_useMaxDT = false;
        float                   m_maxDT = 1000.0f / 60.0f;
        bool                    m_useFixedDT = false;
        float                   m_fixedDT = 1000.0f / 60.0f;

        // Animation
        AnimationOptionFlags    m_animationOptionFlags = AnimationOptionFlags::Jobs;
    };
}