#include "renderer/Precomp.h"
#include "LightInstance.h"

#if !VG_ENABLE_INLINE
#include "LightInstance.inl"
#endif

#include "Directional/DirectionalLightInstance.hpp"
#include "Omni/OmniLightInstance.hpp"

using namespace vg::core;
using namespace vg::gfx;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    // LightDesc
    //--------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------
    bool HideCastShadow(const IObject * _object, const IProperty * _prop, uint _index)
    {
        const LightDesc * lightDesc = VG_SAFE_STATIC_CAST(const LightDesc, _object);
        return lightDesc->GetLightType() != LightType::Directional;
    }

    //--------------------------------------------------------------------------------------
    bool LightDesc::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEx(LightDesc, m_shadow, "Cast Shadows", PropertyFlags::Hidden);

        registerPropertyOptionalGroupBegin(LightDesc, m_shadow, "Shadows");
        {
            registerProperty(LightDesc, m_shadowRange, "Range");
            setPropertyRange(LightDesc, m_shadowRange, float2(0.0f, 1000.0f));
            setPropertyDescription(LightDesc, m_shadowRange, "Max shadow distance");

            registerProperty(LightDesc, m_shadowBias, "Bias");
            setPropertyRange(LightDesc, m_shadowBias, float2(0.0f, 1.0f));
            setPropertyDescription(LightDesc, m_shadowBias, "Bias shadow distance (in cm)");

            registerProperty(LightDesc, m_shadowSize, "Size");
            setPropertyDescription(LightDesc, m_shadowSize, "Shadow map size in world-space");

            registerPropertyEnum(LightDesc, ShadowResolution, m_shadowResolution, "Resolution");
            setPropertyDescription(LightDesc, m_shadowResolution, "Shadow map resolution from 'VeryLow' (default size /4) to 'Very High' (default size x4)");

            registerProperty(LightDesc, m_shadowIntensity, "Shadow Intensity");
            setPropertyRange(LightDesc, m_shadowIntensity, float2(0.0f, 1.0f));
            setPropertyDescription(LightDesc, m_shadowIntensity, "Amound of light occluded in shadowed areas");

            registerPropertyEnum(LightDesc, ShadowCameraMode, m_shadowCameraMode, "Camera mode");
            setPropertyDescription(LightDesc, m_shadowCameraMode, "How shadow behaves when camera moves/rotates");
        }
        registerPropertyOptionalGroupEnd(LightDesc);
        setPropertyHiddenCallback(LightDesc, Shadows, HideCastShadow);

        registerPropertyEx(LightDesc, m_color, "Color", PropertyFlags::Color | PropertyFlags::HDR);
        setPropertyDescription(LightDesc, m_color, "Direct Light color");

        registerProperty(LightDesc, m_intensity, "Intensity");
        setPropertyDescription(LightDesc, m_intensity, "Direct Light intensity multiplier");
        setPropertyRange(LightDesc, m_intensity, float2(0.0f, 100.0f));

        return true;
    }

    //--------------------------------------------------------------------------------------
    // LightInstance
    //--------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------
    bool LightInstance::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    LightInstance::LightInstance(const core::string & _name, core::IObject * _parent, const LightDesc * _lightDesc) :
        super(_name, _parent)
    {
        m_shadow = _lightDesc->m_shadow;
        m_shadowCameraMode = _lightDesc->m_shadowCameraMode;
        m_shadowRange = _lightDesc->m_shadowRange;
        m_shadowBias = _lightDesc->m_shadowBias * 0.01f;
        m_shadowSize = _lightDesc->m_shadowSize;
        m_shadowResolution = _lightDesc->m_shadowResolution;
        m_shadowIntensity = m_shadow ? _lightDesc->m_shadowIntensity : 0.0f;

        setColor(_lightDesc->m_color);
        m_intensity = _lightDesc->m_intensity;
    }

    //--------------------------------------------------------------------------------------
    LightInstance::~LightInstance()
    {
        
    }

    //--------------------------------------------------------------------------------------
    bool LightInstance::IsCastShadow() const 
    { 
        if (m_shadow)
        {
            const auto * rendererOptions = RendererOptions::get();
            return rendererOptions->IsShadowEnabled();
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    core::uint2 LightInstance::getShadowResolution() const
    {
        const auto * rendererOptions = RendererOptions::get();
        uint2 resolution = rendererOptions->GetShadowDefaultResolution();
        const int shadowResInt = asInteger(m_shadowResolution);
        if (shadowResInt > 0)
            resolution <<= shadowResInt;
        else if (shadowResInt < 0)
            resolution >>= (uint)(-shadowResInt);

        return resolution;
    }

    //--------------------------------------------------------------------------------------
    bool LightInstance::Cull(const ViewCullingOptions & _cullingOptions, CullingResult * _cullingResult)
    {
        _cullingResult->m_output->add(GetLightType(), this);
        _cullingResult->m_output->add(GraphicInstanceListType::All, this);

        if (asBool(ObjectRuntimeFlags::Selected & getObjectRuntimeFlags()))
            _cullingResult->m_output->add(GraphicInstanceListType::Outline, this);

        return true;
    }

    //--------------------------------------------------------------------------------------
    void LightInstance::Draw(const RenderContext & _renderContext, gfx::CommandList * _cmdList) const
    {
        switch (_renderContext.m_shaderPass)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(_renderContext.m_shaderPass);
                break;

            case ShaderPass::ZOnly:
                break;

            case ShaderPass::Forward:
            case ShaderPass::Deferred:
                DebugDraw::get()->drawCube(_cmdList, DebugDrawFillMode::Solid, true, (float3)-0.05f, (float3)0.05f, packRGBA8(GetColor()), getGlobalMatrix(), GetPickingID());
                break;
        }
    }
}