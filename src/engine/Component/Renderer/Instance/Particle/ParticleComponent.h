#pragma once

#include "core/Component/Component.h"
#include "ParticleEmitterList.h"

namespace vg::renderer
{
    class IParticleSystemInstance;
}

namespace vg::engine
{
    class ParticleComponent : public core::Component
    {
    public:
        VG_CLASS_DECL(ParticleComponent, core::Component);

        ParticleComponent(const core::string & _name, IObject * _parent);
        ~ParticleComponent();

        void                                OnLoad() final override;
        void                                Update(const Context & _context) override;
        
        void                                OnPropertyChanged(IObject * _object, const core::IProperty & _prop, bool _notifyParent) override;
        void                                EnableComponentFlags(core::ComponentFlags _flags, bool _enabled) final override;
        
        const core::string                  GetSubObjectName(core::uint _subObjectIndex) const override;
        
        void                                OnResourceLoaded(core::IResource * _resource) override;
        void                                OnResourceUnloaded(core::IResource * _resource) override;
        
        bool                                TryGetAABB(core::AABB & _aabb) const final override;
        
        bool                                isSkeletonVisible() const;

        VG_INLINE renderer::IParticleSystemInstance * getParticleSystemInstance() const;

    private:
        renderer::IParticleSystemInstance * m_particleSystemInstance = nullptr;
        bool                                m_registered = false;
        core::u16                           m_maxParticleCount = 1;
        ParticleEmitterList                 m_particleEmitters;
    };
}

#if VG_ENABLE_INLINE
#include "ParticleComponent.inl"
#endif