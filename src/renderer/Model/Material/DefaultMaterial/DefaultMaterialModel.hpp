#include "DefaultMaterialModel.h"
#include "Shaders/default/default.hlsl.h"
#include "Shaders/system/materialdata.hlsli"
#include "renderer/Model/Material/MaterialManager.h"
#include "renderer/Options/RendererOptions.h"

namespace vg::renderer
{
    using namespace vg::core;
    using namespace vg::gfx;

    VG_REGISTER_OBJECT_CLASS_EX(DefaultMaterialModel, "Default", ClassDescFlags::Model);

    //--------------------------------------------------------------------------------------
    bool DefaultMaterialModel::registerProperties(core::IClassDesc & _desc)
    {
        registerPropertyEnumEx(DefaultMaterialModel, UVSource, m_UVSource, "UV Source", PropertyFlags::NotSaved);

        registerPropertyEx(DefaultMaterialModel, m_tiling, "Tiling", PropertyFlags::NotSaved);
        setPropertyRange(DefaultMaterialModel, m_tiling, float2(0, 16));

        registerPropertyEx(DefaultMaterialModel, m_enableAlbedo, "Enable Albedo", PropertyFlags::NotSaved);
        registerPropertyObjectEx(DefaultMaterialModel, m_albedoMap, "Albedo Map", PropertyFlags::NotSaved);
        registerPropertyEx(DefaultMaterialModel, m_albedoColor, "Albedo Color", PropertyFlags::Color);

        registerPropertyEx(DefaultMaterialModel, m_enableNormal, "Enable Normal", PropertyFlags::NotSaved);
        registerPropertyObjectEx(DefaultMaterialModel, m_normalMap, "Normal Map", PropertyFlags::NotSaved);
        registerPropertyEx(DefaultMaterialModel, m_normalStrength, "Normal Strength", PropertyFlags::NotSaved);
        setPropertyRange(DefaultMaterialModel, m_normalStrength, float2(0.0f, 1.0f));

        registerPropertyEx(DefaultMaterialModel, m_enablePbr, "Enable PBR", PropertyFlags::NotSaved);
        registerPropertyObjectEx(DefaultMaterialModel, m_pbrMap, "PBR Map", PropertyFlags::NotSaved);
        registerPropertyEx(DefaultMaterialModel, m_occlusion, "Occlusion", PropertyFlags::NotSaved);
        setPropertyRange(DefaultMaterialModel, m_occlusion, float2(0.0f, 1.0f));
        registerPropertyEx(DefaultMaterialModel, m_roughness, "Roughness", PropertyFlags::NotSaved);
        setPropertyRange(DefaultMaterialModel, m_roughness, float2(0.0f, 1.0f));
        registerPropertyEx(DefaultMaterialModel, m_metalness, "Metalness", PropertyFlags::NotSaved);
        setPropertyRange(DefaultMaterialModel, m_metalness, float2(0.0f, 1.0f));

        return super::registerProperties(_desc);
    }

    //--------------------------------------------------------------------------------------
    DefaultMaterialModel::DefaultMaterialModel(const core::string & _name, IObject * _parent) :
        MaterialModel(_name, _parent)
    {
        // Register slot for material GPU data
        m_gpuMaterialDataIndex = Renderer::get()->getMaterialManager()->allocMaterialDataGPUIndex(this);
    }

    //--------------------------------------------------------------------------------------
    DefaultMaterialModel::~DefaultMaterialModel()
    {
        // Unregister slot for material GPU data
        Renderer::get()->getMaterialManager()->freeMaterialDataGPUIndex(m_gpuMaterialDataIndex);
    }

    //--------------------------------------------------------------------------------------
    void DefaultMaterialModel::FillGPUMaterialData(GPUMaterialData * _data) const
    {
        _data->setUVSource(m_UVSource);
        _data->setTiling(m_tiling);

        //const auto lightingMode = RendererOptions::get()->getLightingMode();
        const bool enableAlbedo = (/*LightingMode::Deferred == lightingMode && */ SurfaceType::Decal == m_surfaceType) | m_enableAlbedo;

        _data->setAlbedoTextureHandle(enableAlbedo && m_albedoMap ? m_albedoMap->getTextureHandle() : RESERVEDSLOT_TEXSRV_DEFAULT_ALBEDO);
        _data->setAlbedoColor(m_albedoColor);

        _data->setNormalTextureHandle(m_enableNormal && m_normalMap ? m_normalMap->getTextureHandle() : RESERVEDSLOT_TEXSRV_DEFAULT_NORMAL);
        _data->setNormalStrength(m_normalStrength);

        _data->setPBRTextureHandle(m_enablePbr && m_pbrMap ? m_pbrMap->getTextureHandle() : RESERVEDSLOT_TEXSRV_DEFAULT_PBR);
        _data->setOcclusion(m_occlusion);
        _data->setRoughness(m_roughness);
        _data->setMetalness(m_metalness);
    }

    //--------------------------------------------------------------------------------------
    void DefaultMaterialModel::Setup(const RenderContext & _renderContext, gfx::CommandList * _cmdList, RootConstants3D * _root3D, core::uint _index) const
    {
        _root3D->setMatID(_index);
        //_root3D->setColor(m_albedoColor);

        auto key = m_shaderKey[asInteger(_renderContext.m_shaderPass)];

        RasterizerState rs(FillMode::Solid, CullMode::Back);

        _cmdList->setGraphicRootSignature(m_rootSignature);
        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleList);

        switch (_renderContext.m_shaderPass)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(_renderContext.m_shaderPass);
                break;

            case ShaderPass::ZOnly:
            {
                BlendState bs(BlendFactor::Zero, BlendFactor::Zero, BlendOp::Add, (ColorWrite)0x0);
                _cmdList->setBlendState(bs);

                key.setFlag(gfx::DefaultHLSLDesc::Toolmode, false);
                key.setFlag(gfx::DefaultHLSLDesc::ZOnly, true);
            }
            break;

            case ShaderPass::Forward:
            case ShaderPass::Transparent:
            {
                BlendState bs;
                if (_renderContext.m_shaderPass == ShaderPass::Forward && _renderContext.m_surfaceType != SurfaceType::Decal)
                    bs = BlendState(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);
                else
                    bs = BlendState(BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha, BlendOp::Add);

                // TODO: find a blend mode + shader output to modulate luminosity?
                //if (SurfaceType::Decal == m_surfaceType)
                //{
                //    if (!m_enableAlbedo)
                //        bs = BlendState(BlendFactor::SrcAlpha, BlendFactor::One, BlendOp::Mul);
                //}

                _cmdList->setBlendState(bs);

                if (_renderContext.m_raytracing)
                    key.setFlag(gfx::DefaultHLSLDesc::RayTracing, true);

                if (_renderContext.m_toolmode)
                {
                    if (_renderContext.m_wireframe)
                    {
                        rs = RasterizerState(FillMode::Wireframe, CullMode::None);
                        _root3D->setFlags(RootConstantsFlags::Wireframe);
                    }

                    key.setFlag(gfx::DefaultHLSLDesc::Toolmode, true);
                }
                else
                {
                    key.setFlag(gfx::DefaultHLSLDesc::Toolmode, false);
                }
            }
            break;

            case ShaderPass::Deferred:
            {
                BlendState bs;
                if (_renderContext.m_surfaceType == SurfaceType::Decal)
                    bs = BlendState(BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha, BlendOp::Add);
                else
                    bs = BlendState(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);

                if (SurfaceType::Decal == m_surfaceType)
                {
                    if (!m_enableAlbedo)
                    {
                        bs.m_renderTargetBlend[0].colorWrite = (gfx::ColorWrite)0x0;
                        bs.m_flags = gfx::BlendStateFlags::IndependantBlend;
                    }

                    //if (!m_enableNormal)
                    //{
                    //    bs.m_renderTargetBlend[1].colorWrite = (gfx::ColorWrite)0x0;
                    //    bs.m_flags = gfx::BlendStateFlags::IndependantBlend;
                    //}
                    //
                    //if (!m_enablePbr)
                    //{
                    //    bs.m_renderTargetBlend[2].colorWrite = (gfx::ColorWrite)0x0;
                    //    bs.m_flags = gfx::BlendStateFlags::IndependantBlend;
                    //}
                }
                    
                _cmdList->setBlendState(bs);

                VG_ASSERT(!_renderContext.m_wireframe);

                if (_renderContext.m_toolmode)
                    key.setFlag(gfx::DefaultHLSLDesc::Toolmode, true);
                else
                    key.setFlag(gfx::DefaultHLSLDesc::Toolmode, false);
            }
            break;
        }       

        gfx::CullMode cullMode;
        switch (m_cullMode)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(m_cullMode);
                break;
            case CullMode::None:
                cullMode = CullMode::None;
                break;
            case CullMode::Back:
                cullMode = CullMode::Back;
                break;
            case CullMode::Front:
                cullMode = CullMode::Front;
                break;

        }
        rs = RasterizerState(rs.getFillMode(), cullMode);

        _cmdList->setRasterizerState(rs);

        key.setFlags(gfx::DefaultHLSLDesc::Surface, (uint)m_surfaceType);

        _cmdList->setShader(key);
    }

    //--------------------------------------------------------------------------------------
    void DefaultMaterialModel::SetSurfaceType(gfx::SurfaceType _surfaceType)
    {
        m_surfaceType = _surfaceType;
    }

    //--------------------------------------------------------------------------------------
    void DefaultMaterialModel::SetColor(const core::string & _name, core::float4 _value)
    {
        if (_name == "AlbedoColor")
            m_albedoColor = _value;
    }

    //--------------------------------------------------------------------------------------
    void DefaultMaterialModel::SetTexture(const core::string & _name, gfx::ITexture * _value)
    {
        if (_name == "m_albedoMap")
            assignTexture((gfx::Texture**)&m_albedoMap, (gfx::Texture *)_value);
        else if (_name == "m_normalMap")
            assignTexture((gfx::Texture**)&m_normalMap, (gfx::Texture *)_value);
        else if (_name == "m_pbrMap")
            assignTexture((gfx::Texture **)&m_pbrMap, (gfx::Texture *)_value);
    }

    //--------------------------------------------------------------------------------------
    void DefaultMaterialModel::assignTexture(gfx::Texture ** _texture, gfx::Texture * _value)
    {
        if (*_texture != _value)
        {
            VG_SAFE_RELEASE_ASYNC(*_texture);
            VG_SAFE_INCREASE_REFCOUNT(_value);
            *_texture = _value;
        }
    }

    //--------------------------------------------------------------------------------------
    void DefaultMaterialModel::SetFloat(const core::string & _name, float _value)
    {
        if (_name == "NormalStrength")
            m_normalStrength = _value;
        else if (_name == "Occlusion")
            m_occlusion = _value;
        else if (_name == "Roughness")
            m_roughness = _value;
        else if (_name == "Metalness")
            m_metalness = _value;
    }

    //--------------------------------------------------------------------------------------
    void DefaultMaterialModel::SetFloat2(const core::string & _name, core::float2 _value)
    {
        if (_name == "Tiling")
            m_tiling = _value;
    }
}