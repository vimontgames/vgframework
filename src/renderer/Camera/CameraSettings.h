#pragma once
#include "renderer/ICameraSettings.h"

namespace vg::renderer
{
    class CameraSettings final: public ICameraSettings
    {
    public:
        VG_CLASS_DECL(CameraSettings, ICameraSettings);

        CameraSettings(const core::string & _name, core::IObject * _parent = nullptr);
        ~CameraSettings();

        void                OnLoad          ();

        const ICameraLens * GetCameraLens   () const final override;
        float               GetFocalLength  () const final override;
        GateFitMode    GetGateFitMode  () const final override;
        float               GetNear         () const final override;
        float               GetFar          () const final override;

    private:
        float               m_near = 0.1f;
        float               m_far = 1000.0f;
        core::IResource *   m_lensRes = nullptr;
        float               m_focalLength = 35.0f;
        float               m_focusDistance = 3.0f;
        GateFitMode    m_gateFitMode = GateFitMode::Vertical;
        float               m_aperture = 4.0f;
    };
}