#pragma once
#include "core/Singleton/Singleton.h"
#include "core/IGameObject.h"
#include "core/Memory/MemoryInfo.h"
#include "engine/IEngineOptions.h"
#include "physics/Physics_Consts.h"

namespace vg::physics
{
    enum class ShapeType : core::u8;
    enum class ShapeTypeFlags : core::u32;
}

namespace vg::engine
{
    vg_enum_class(vg::engine, AnimationOptionFlags, core::u32,
        AnimationJobs = 0x00000001,
        ShowSkeletons = 0x00000002        
    );

    vg_enum_class(vg::engine, LoadingOptionFlags, core::u32,
        ResourcePriority = 0x00000001
    );

    class EngineOptions final : public IEngineOptions, public core::Singleton<EngineOptions>
    {
    public:
        VG_CLASS_DECL(EngineOptions, IEngineOptions);

        EngineOptions(const core::string & _name, core::IObject * _parent = nullptr);

        const core::string &        GetProjectPath              () const final override;
        const core::string &        GetStartWorld               () const final override;
        float                       TryGetTimeScale             (float & _timescale) const final override;
        bool                        TryGetFixedDT               (float & _fixedDT) const final override;
        bool                        TryGetMaxDT                 (float & _maxDT) const final override;
        void                        OnPropertyChanged           (IObject * _object, const core::IProperty & _prop, bool _notifyParent) final override;

        core::Tag                   GetGameObjectTag            (const core::string & _name) const final override;
        physics::Category           GetPhysicsCategory          (const core::string & _name) const final override;

        void                        SetCpuMemoryInfo            (const core::CPUMemoryInfo & _cpuMem) final override;
        const core::CPUMemoryInfo & GetCpuMemoryInfo            () const final override;
        void                        SetGpuMemoryInfo            (const core::GPUMemoryInfo & _gpuMem) final override;
        const core::GPUMemoryInfo & GetGpuMemoryInfo            () const final override;

        core::vector<core::string>  getPhysicsCategoryNames     () const;

        VG_INLINE bool              useAnimationJobs            () const { return core::asBool(core::operator & (AnimationOptionFlags::AnimationJobs, m_animationOptionFlags)); }
        VG_INLINE bool              isShowSkeleton              () const { return core::asBool(core::operator & (AnimationOptionFlags::ShowSkeletons, m_animationOptionFlags)); }

        VG_INLINE bool              isAnyBodyVisible            () const { return 0 != m_showRigidBodies; }
        VG_INLINE bool              isBodyVisible               (physics::ShapeType _shape) const { return m_showRigidBodies && ((1 << (core::u32)_shape) & (core::u32)m_showRigidBodiesMask); }
        VG_INLINE bool              mergeStaticBodies           () const { return m_mergeStaticBodies; }

        VG_INLINE bool              useResourceLoadingPriority  () const { return core::asBool(core::operator & (LoadingOptionFlags::ResourcePriority, m_loadingOptionFlags)); }

    private:
        void                        updateDynamicEnum           (const core::IProperty & _prop);

    private:
        // Project
        core::string                m_gamePath;
        core::string                m_startWorld;
        vg::core::string            m_gameObjectTags[core::enumCount<core::Tag>()];

        // Physics
        bool                        m_mergeStaticBodies = false;
        bool                        m_showRigidBodies = false;
        physics::ShapeTypeFlags     m_showRigidBodiesMask = (physics::ShapeTypeFlags)-1;
        vg::core::string            m_physicsCategories[core::enumCount<physics::Category>()];

        // Time
        bool                        m_useTimeScale = false;
        float                       m_timeScale    = 1.0f;
        bool                        m_useMaxDT     = false;
        float                       m_maxDT        = 1000.0f / 60.0f;
        bool                        m_useFixedDT   = false;
        float                       m_fixedDT      = 1000.0f / 60.0f;

        // Animation
        AnimationOptionFlags        m_animationOptionFlags = AnimationOptionFlags::AnimationJobs;

        // Loading
        LoadingOptionFlags          m_loadingOptionFlags = LoadingOptionFlags::ResourcePriority;

        // Memory
        core::CPUMemoryInfo         m_cpuMemory;
        core::GPUMemoryInfo         m_gpuMemory;
    };
}