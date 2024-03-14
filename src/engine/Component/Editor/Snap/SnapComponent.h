#pragma once
#include "engine/ISnapComponent.h"

namespace vg::engine
{
    class SnapComponent : public ISnapComponent
    {
    public:
        VG_CLASS_DECL(SnapComponent, ISnapComponent);

        SnapComponent(const core::string & _name, core::IObject * _parent);
        ~SnapComponent();

        bool            TryGetSnapTranslate (core::float3 & _translate) const final override;
        bool            TryGetSnapRotate    (float & _rotate) const final override;
        bool            TryGetSnapScale     (float & _scale) const final override;

        void            OnPropertyChanged   (core::IObject * _object, const core::IProperty & _prop, bool _notifyParent);

    protected:
        bool            m_snapTranslation = true;
        core::float3    m_translation = core::float3(1,1,1);
        bool            m_snapRotation = false;
        float           m_rotation = 45.0f;
        bool            m_snapScale = false;
        float           m_scale = 1.0f;
    };
}