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

        void                    OnPropertyChanged   (IObject * _object, const core::IProperty & _prop, bool _notifyParent) final override;

        static bool			    load                (core::IObject * _object);
        static bool			    save                (core::IObject * _object);

    private:
        core::string            m_projectPath;
        core::string            m_startWorld;
    };
}