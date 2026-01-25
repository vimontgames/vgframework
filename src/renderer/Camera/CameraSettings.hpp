#include "CameraSettings.h"
#include "core/IResource.h"
#include "renderer/Renderer.h"

using namespace vg::core;

namespace vg::renderer
{
    VG_REGISTER_OBJECT_CLASS(CameraSettings, "Camera Settings");

    //--------------------------------------------------------------------------------------
    float2 getFocalLengthRange(const IObject * _object, const IProperty * _prop, uint _index)
    {
        auto * cameraSettings = VG_SAFE_STATIC_CAST(const CameraSettings, _object);
        const renderer::ICameraLens * lens = cameraSettings->GetCameraLens();
        return lens->GetFocalLengthRange();
    }
    
    //--------------------------------------------------------------------------------------
    float2 getFocusDistanceRange(const IObject * _object, const IProperty * _prop, uint _index)
    {
        auto * cameraSettings = VG_SAFE_STATIC_CAST(const CameraSettings, _object);
        const renderer::ICameraLens * lens = cameraSettings->GetCameraLens();
        return float2(lens->GetMinFocusDistance(cameraSettings->GetFocalLength()), 1000.0f);
    }
    
    //--------------------------------------------------------------------------------------
    float2 getApertureRange(const IObject * _object, const IProperty * _prop, uint _index)
    {
        auto * cameraSettings = VG_SAFE_STATIC_CAST(const CameraSettings, _object);
        const renderer::ICameraLens * lens = cameraSettings->GetCameraLens();
        const float focalLength = cameraSettings->GetFocalLength();
        return float2(lens->GetMaxAperture(focalLength), lens->GetMinAperture(focalLength));
    }

    //--------------------------------------------------------------------------------------
    bool CameraSettings::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyGroupBegin(CameraSettings, "Range");
        {
            registerProperty(CameraSettings, m_near, "Near");
            setPropertyRange(CameraSettings, m_near, float2(0.0f, 10.0f));
            setPropertyDescription(CameraSettings, m_near, "Camera near distance");
        
            registerProperty(CameraSettings, m_far, "Far");
            setPropertyRange(CameraSettings, m_far, float2(0.0f, 10000.0f));
            setPropertyDescription(CameraSettings, m_far, "Camera far distance");
        }
        registerPropertyGroupEnd(CameraSettings);
        
        registerPropertyGroupBegin(CameraSettings, "Optics");
        {
            registerPropertyResourcePtr(CameraSettings, m_lensRes, "Lens");
        
            registerProperty(CameraSettings, m_focalLength, "Focal length");
            setPropertyDescription(CameraSettings, m_focalLength, "Focal length in millimeters");
            setPropertyRangeCallback(CameraSettings, m_focalLength, getFocalLengthRange); // Range depends on the lens used
        
            registerProperty(CameraSettings, m_focusDistance, "Focus distance");
            setPropertyDescription(CameraSettings, m_focusDistance, "The distance in meters between the lens and the subject that is in sharp focus");
            setPropertyRangeCallback(CameraSettings, m_focusDistance, getFocusDistanceRange); // Range depends on the lens used and focal length
        
            registerProperty(CameraSettings, m_aperture, "f/Aperture");
            setPropertyDescription(CameraSettings, m_aperture, "A higher f-stop (e.g., f/16) means a smaller aperture that lets in less light, creating a deeper depth of field");
            setPropertyRangeCallback(CameraSettings, m_aperture, getApertureRange); // Range depends on the lens used and focal length
        
            registerPropertyEnum(CameraSettings, GateFitMode, m_gateFitMode, "Gate Fit");
            setPropertyDescription(CameraSettings, m_gateFitMode, "Determines how the camera aligns its field of view to fit the sensor");
        }
        registerPropertyGroupEnd(CameraSettings);

        registerPropertyGroupBegin(CameraSettings, "Post-processing");
        {
            registerProperty(CameraSettings, m_outlines, "Outlines");
            registerProperty(CameraSettings, m_depthOfField, "Depth-of-field");
            registerPropertyEx(CameraSettings, m_motionBlur, "Motion blur", PropertyFlags::ReadOnly);

            registerOptionalPropertyEx(CameraSettings, m_fade, m_fadeColor, "Fade", PropertyFlags::Color);            
        }
        registerPropertyGroupEnd(CameraComponent);

        return true;
    }

    //--------------------------------------------------------------------------------------
    CameraSettings::CameraSettings(const string & _name, IObject * _parent) :
        super(_name, _parent)
    {
        m_lensRes = (core::IResource*)Kernel::getFactory()->CreateObject("LensResource");
        m_lensRes->SetParent(this);
    }

    //--------------------------------------------------------------------------------------
    CameraSettings::~CameraSettings()
    {
        VG_SAFE_RELEASE(m_lensRes);
    }

    //--------------------------------------------------------------------------------------
    void CameraSettings::OnLoad()
    {
        RegisterUID();
    }

    //--------------------------------------------------------------------------------------
    void CameraSettings::SetZoomLevel(float _zoom)
    {
        if (auto * camLens = VG_SAFE_STATIC_CAST(renderer::ICameraLens, m_lensRes->GetObject()))
        {
            float2 range = camLens->GetFocalLengthRange();
            m_focalLength = lerp(range.x, range.y, _zoom);
        }
    }

    //--------------------------------------------------------------------------------------
    const ICameraLens * CameraSettings::GetCameraLens() const
    {
        if (auto * camLens = VG_SAFE_STATIC_CAST(renderer::ICameraLens, m_lensRes->GetObject()))
            return camLens;
        else
            return Renderer::get()->GetDefaultCameraLens();
    }

    //--------------------------------------------------------------------------------------
    float CameraSettings::GetFocalLength() const
    {
        return m_focalLength;
    }

    //--------------------------------------------------------------------------------------
    float CameraSettings::GetAperture() const
    {
        return m_aperture;
    }

    //--------------------------------------------------------------------------------------
    float CameraSettings::GetFocusDistance() const
    {
        return m_focusDistance;
    }

    //--------------------------------------------------------------------------------------
    GateFitMode CameraSettings::GetGateFitMode() const
    {
        return m_gateFitMode;
    }

    //--------------------------------------------------------------------------------------
    float CameraSettings::GetNear() const
    {
        return m_near;
    }

    //--------------------------------------------------------------------------------------
    float CameraSettings::GetFar() const
    {
        return m_far;
    }

    //--------------------------------------------------------------------------------------
    bool CameraSettings::IsOutlinesEnabled() const
    {
        return m_outlines;
    }

    //--------------------------------------------------------------------------------------
    bool CameraSettings::IsDepthOfFieldEnabled() const
    {
        return m_depthOfField;
    }

    //--------------------------------------------------------------------------------------
    bool CameraSettings::IsMotionBlurEnabled() const
    {
        return m_motionBlur;
    }

    //--------------------------------------------------------------------------------------
    core::float4 CameraSettings::GetFadeColor() const
    {
        if (m_fade)
            return m_fadeColor;
        else
            return float4(0,0,0, 0);
    }
}

