#include "CameraLens.h"

using namespace vg::core;

namespace vg::renderer
{
    VG_REGISTER_OBJECT_CLASS(CameraLens, "Camera Lens");

    //--------------------------------------------------------------------------------------
    bool CameraLens::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        setPropertyFlag(CameraLens, m_name, PropertyFlags::NotVisible, false);

        registerProperty(CameraLens, m_sensorSize, "Sensor size");
        setPropertyRange(CameraLens, m_sensorSize, core::float2(3.6f, 100.0f));
        setPropertyDescription(CameraLens, m_sensorSize, "Sensor size is the physical dimensions of a camera's sensor");

        registerProperty(CameraLens, m_focalLength, "Zoom range");
        setPropertyRange(CameraLens, m_focalLength, core::float2(10.0f, 400.0f));
        setPropertyDescription(CameraLens, m_focalLength, "The focal length range defines a lens's zoom capabilities, from wide-angle to telephoto");

        registerProperty(CameraLens, m_minFocusDistance, "MFD");
        setPropertyRange(CameraLens, m_minFocusDistance, core::float2(0.3f, 3.0f));
        setPropertyDescription(CameraLens, m_minFocusDistance, "The minimum focus distance, from wide-angle to telephoto");

        registerProperty(CameraLens, m_minAperture, "Min f/Aperture");
        setPropertyRange(CameraLens, m_minAperture, core::float2(22.0f, 16.0f));
        setPropertyDescription(CameraLens, m_minAperture, "The minimum aperure size, from wide-angle to telephoto");

        registerProperty(CameraLens, m_maxAperture, "Max f/Aperture");
        setPropertyRange(CameraLens, m_maxAperture, core::float2(1.4f, 6.3f));
        setPropertyDescription(CameraLens, m_maxAperture, "The minimum aperure size, from wide-angle to telephoto");

        return true;
    }

    //--------------------------------------------------------------------------------------
    CameraLens::CameraLens(const string & _name, IObject * _parent) :
        super(_name, _parent)
    {
      
    }

    //--------------------------------------------------------------------------------------
    CameraLens::~CameraLens()
    {

    }

    //--------------------------------------------------------------------------------------
    float CameraLens::GetZoomFactor(float _focalLength) const
    {
        const float zoomRatio = (m_focalLength.y / m_focalLength.x);
        return lerp(1.0f, zoomRatio, getFocalLengthBlendFactor(_focalLength));
    }

    //--------------------------------------------------------------------------------------
    float CameraLens::GetMinFocusDistance(float _focalLength) const
    {
        return lerp(m_minFocusDistance.x, m_minFocusDistance.y, getFocalLengthBlendFactor(_focalLength));
    }

    //--------------------------------------------------------------------------------------
    float CameraLens::GetMinAperture(float _focalLength) const
    {
        return lerp(m_minAperture.x, m_minAperture.y, getFocalLengthBlendFactor(_focalLength));
    }

    //--------------------------------------------------------------------------------------
    float CameraLens::GetMaxAperture(float _focalLength) const
    {
        return lerp(m_maxAperture.x, m_maxAperture.y, getFocalLengthBlendFactor(_focalLength));
    }
 
    //--------------------------------------------------------------------------------------
    float CameraLens::getFocalLengthBlendFactor(float _focalLength) const
    {
        if (((float)m_focalLength.y - (float)m_focalLength.x) <= 0.0f)
            return 0.0f; // Fixed focal length
        else
            return saturate(_focalLength / (m_focalLength.y - m_focalLength.x));
    }
}

