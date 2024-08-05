#pragma once
#include "core/Singleton/Singleton.h"
#include "engine/IEngineOptions.h"

namespace vg::engine
{
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

        VG_INLINE bool          useAnimationJobs    () const { return m_animationJobs;}



    private:
        // Project
        core::string            m_projectPath;
        core::string            m_startWorld;

        // Time
        bool                    m_useTimeScale = false;
        float                   m_timeScale = 1.0f;

        bool                    m_useMaxDT = false;
        float                   m_maxDT = 1000.0f / 60.0f;

        bool                    m_useFixedDT = false;
        float                   m_fixedDT = 1000.0f / 60.0f;

        // Animation
        bool                    m_animationJobs = false;
    };
}