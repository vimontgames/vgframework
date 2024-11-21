#pragma once

#include "renderer/ICameraLens.h"

namespace vg::renderer
{
    class CameraLens : public ICameraLens
    {
    public:
        VG_CLASS_DECL(CameraLens, ICameraLens);

        CameraLens(const core::string & _name, core::IObject * _parent = nullptr);
        ~CameraLens();

        const core::float2 &    GetSensorSize               () const final override { return m_sensorSize; }
        const core::float2 &    GetFocalLengthRange         () const final override { return m_focalLength; }
        const core::float2 &    GetMinFocusDistanceRange    () const final override { return m_minFocusDistance; }
        const core::float2 &    GetMinApertureRange         () const final override { return m_minAperture; }
        const core::float2 &    GetMaxApertureRange         () const final override { return m_maxAperture; }

        float                   GetZoomFactor               (float _focalLength) const final override;
        float                   GetMinFocusDistance         (float _focalLength) const final override;
        float                   GetMinAperture              (float _focalLength) const final override;
        float                   GetMaxAperture              (float _focalLength) const final override;

    private:
        float                   getFocalLengthBlendFactor   (float _focalLength) const;

    private:
        core::float2        m_sensorSize = core::float2(36.0f, 24.0f);
        core::float2        m_focalLength = core::float2(24.0f, 70.0f);
        core::float2        m_minFocusDistance = core::float2(0.38f, 0.38f);
        core::float2        m_minAperture = core::float2(22.0f, 22.0f);
        core::float2        m_maxAperture = core::float2(3.5f, 5.6f);
    };
}