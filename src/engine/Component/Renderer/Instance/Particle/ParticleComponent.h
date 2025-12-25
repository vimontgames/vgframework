#pragma once

#include "engine/IParticleComponent.h"
#include "ParticleEmitterList.h"

namespace vg::renderer
{
    class IParticleSystemInstance;
}

namespace vg::engine
{
    class ParticleComponent : public IParticleComponent
    {
    public:
        VG_CLASS_DECL(ParticleComponent, IParticleComponent);

        ParticleComponent(const core::string & _name, IObject * _parent);
        ~ParticleComponent();

        void                                            OnLoad                      () final override;
        void                                            Update                      (const Context & _context) override;

        void                                            OnPlay                      () override;
        void                                            OnStop                      () override;
        
        void                                            OnPropertyChanged           (IObject * _object, const core::IProperty & _prop, bool _notifyParent) override;
        void                                            EnableComponentFlags        (core::ComponentFlags _flags, bool _enabled) final override;
        
        const core::string                              GetSubObjectName            (core::uint _subObjectIndex) const override;
        
        void                                            OnResourceLoaded            (core::IResource * _resource) override;
        void                                            OnResourceUnloaded          (core::IResource * _resource) override;
        
        bool                                            TryGetAABB                  (core::AABB & _aabb) const final override;

        renderer::IGraphicInstance *                    GetGraphicInstance          () { return m_particleSystemInstance; }

        bool                                            Play                        () final override;
        bool                                            Pause                       () final override;
        bool                                            Stop                        () final override;
        bool                                            Reset                       () final override;

        bool                                            UpdateEmitter               (core::uint _index) final override;
        bool                                            ResetEmitter                (core::uint _index) final override;
        
        bool                                            isSkeletonVisible           () const;

        core::uint                                      getEmitterIndex             (ParticleEmitterDesc * _emitter) const;

        VG_INLINE renderer::IParticleSystemInstance *   getParticleSystemInstance   () const;

        bool                                            onEmittersSwap              (core::uint _indexA, core::uint _indexB);
        bool                                            onEmitterRemoved            (core::uint _index);

        static bool                                     playAllParticleEmitters     (IObject * _object);
        static bool                                     pauseAllParticleEmitters    (IObject * _object);
        static bool                                     stopAllParticleEmitters     (IObject * _object);

    private:
        renderer::IParticleSystemInstance *             m_particleSystemInstance = nullptr;
        bool                                            m_registered = false;
        bool                                            m_playOnStart = false;             
        ParticleEmitterList                             m_particleEmitters;
    };
}

#if VG_ENABLE_INLINE
#include "ParticleComponent.inl"
#endif