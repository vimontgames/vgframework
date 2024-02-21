#pragma once

#include "core/Component/Component.h"
#include "gfx/IView.h"

namespace vg::renderer
{
    enum class LightType : core::u8;
    class ILightDesc;
    class ILightInstance;
}

namespace vg::engine
{
    class LightComponent : public core::Component
    {
    public:
        VG_CLASS_DECL(LightComponent, core::Component);
        LightComponent(const core::string & _name, core::IObject * _parent);
        ~LightComponent();

        void                        OnLoad              () override;
        void                        Update              (float _dt) override;
        void                        OnPropertyChanged   (core::IObject * _object, const core::IProperty & _prop, bool _notifyParent);
        void                        SetComponentFlags   (core::ComponentFlags _flags, bool _enabled = true) final override;

    protected:
        bool                        createLightDesc     ();
        bool                        createLight         ();

    private:
        renderer::LightType         m_lightType;
        renderer::ILightDesc *      m_lightDesc     = nullptr;
        renderer::ILightInstance *  m_light         = nullptr;
        bool                        m_registered    = false;
    };
}

#if VG_ENABLE_INLINE
#include "LightComponent.inl"
#endif