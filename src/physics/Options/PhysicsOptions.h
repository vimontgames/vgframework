#pragma once
#include "core/Singleton/Singleton.h"
#include "physics/IPhysicsOptions.h"

namespace vg::physics
{
    class PhysicsOptions final : public IPhysicsOptions, public core::Singleton<PhysicsOptions>
    {
    public:
        VG_CLASS_DECL(PhysicsOptions, IPhysicsOptions);

        PhysicsOptions(const core::string & _name, core::IObject * _parent = nullptr);

        void                    OnPropertyChanged(core::IObject * _object, const core::IProperty & _prop, bool _notifyParent) final override;
        void                    OnLoad() final override;

        static bool			    load(core::IObject * _object);
        static bool			    save(core::IObject * _object);

    protected:
        void                    applyGravity(const core::float3 & _gravity);

    private:
        core::float3		    m_gravity = core::float3(0.0f, 0.0f, -9.81f);
    };
}