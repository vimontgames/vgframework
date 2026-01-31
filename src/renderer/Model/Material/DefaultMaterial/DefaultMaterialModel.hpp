#include "DefaultMaterialModel.h"
#include "Shaders/default/default.hlsl.h"
#include "Shaders/system/materialdata.hlsli"
#include "renderer/Model/Material/MaterialManager.h"
#include "renderer/Options/RendererOptions.h"

#include "shaders/system/rootConstants3D.hlsli"

namespace vg::renderer
{
    using namespace vg::core;
    using namespace vg::gfx;

    VG_REGISTER_CLASS_EX(DefaultMaterialModel, "Default", ClassDescFlags::Model);

    //--------------------------------------------------------------------------------------
    bool DefaultMaterialModel::registerProperties(core::IClassDesc & _desc)
    {
        // Alpha
        registerPropertyEnum(DefaultMaterialModel, SurfaceType, m_surfaceType, "Surface Type");

        registerPropertyEnum(DefaultMaterialModel, AlphaSource, m_alphaSource, "Alpha Source");
        registerProperty(DefaultMaterialModel, m_depthFade, "Depth transparency");

        // Instance & vertex colors/alpha
        registerPropertyEnumBitfield(DefaultMaterialModel, InstanceColorMask, m_instanceColorMask, "Instance Color");
        registerPropertyEnumBitfield(DefaultMaterialModel, VertexColorMask, m_vertexColorMask, "Vertex Color");

        // Culling
        registerPropertyEnum(DefaultMaterialModel, CullMode, m_cullMode, "Cull");
        
        // UV
        registerPropertyEnumEx(DefaultMaterialModel, UVSource, m_UVSource, "UV Source", PropertyFlags::Transient);

        registerPropertyEx(DefaultMaterialModel, m_tiling, "Tiling", PropertyFlags::Transient);
        setPropertyRange(DefaultMaterialModel, m_tiling, float2(0, 16));

        registerPropertyEx(DefaultMaterialModel, m_offset, "Offset", PropertyFlags::Transient);
        setPropertyRange(DefaultMaterialModel, m_offset, float2(-8, 8));

        // Albedo
        registerPropertyEx(DefaultMaterialModel, m_enableAlbedo, "Enable Albedo", PropertyFlags::Transient);
        registerPropertyObjectEx(DefaultMaterialModel, m_albedoMap, "Albedo Map", PropertyFlags::Transient);
        registerPropertyEx(DefaultMaterialModel, m_albedoColor, "Albedo Color", PropertyFlags::Color);

        // Normal
        registerPropertyEx(DefaultMaterialModel, m_enableNormal, "Enable Normal", PropertyFlags::Transient);
        registerPropertyObjectEx(DefaultMaterialModel, m_normalMap, "Normal Map", PropertyFlags::Transient);
        registerPropertyEx(DefaultMaterialModel, m_normalStrength, "Normal Strength", PropertyFlags::Transient);
        setPropertyRange(DefaultMaterialModel, m_normalStrength, float2(0.0f, 1.0f));

        // PBR
        registerPropertyEx(DefaultMaterialModel, m_enablePbr, "Enable PBR", PropertyFlags::Transient);
        registerPropertyObjectEx(DefaultMaterialModel, m_pbrMap, "PBR Map", PropertyFlags::Transient);
        registerPropertyEx(DefaultMaterialModel, m_occlusion, "Occlusion", PropertyFlags::Transient);
        setPropertyRange(DefaultMaterialModel, m_occlusion, float2(0.0f, 1.0f));
        registerPropertyEx(DefaultMaterialModel, m_roughness, "Roughness", PropertyFlags::Transient);
        setPropertyRange(DefaultMaterialModel, m_roughness, float2(0.0f, 1.0f));
        registerPropertyEx(DefaultMaterialModel, m_metalness, "Metalness", PropertyFlags::Transient);
        setPropertyRange(DefaultMaterialModel, m_metalness, float2(0.0f, 1.0f));

        // Emissive
        registerPropertyEx(DefaultMaterialModel, m_enableEmissive, "Enable Emissive", PropertyFlags::Transient);
        registerPropertyObjectEx(DefaultMaterialModel, m_emissiveMap, "Emissive Map", PropertyFlags::Transient);
        registerPropertyEx(DefaultMaterialModel, m_emissiveColor, "Emissive Color", PropertyFlags::Transient);
        registerPropertyEx(DefaultMaterialModel, m_emissiveIntensity, "Emissive Intensity", PropertyFlags::Transient);

        return super::registerProperties(_desc);
    }

    //--------------------------------------------------------------------------------------
    DefaultMaterialModel::DefaultMaterialModel(const core::string & _name, IObject * _parent) :
        MaterialModel(_name, _parent),
        m_surfaceType(SurfaceType::Opaque),
        m_cullMode(CullMode::Back),
        m_alphaSource(AlphaSource::Albedo),
        m_depthFade(0.1f),
        m_instanceColorMask(InstanceColorMask::Albedo),
        m_vertexColorMask(VertexColorMask::Albedo),
        m_UVSource(UVSource::UV0),
        m_tiling(core::float2(1, 1)),
        m_offset(core::float2(0, 0)),
        m_enableAlbedo(true),
        m_albedoMap(nullptr),
        m_albedoColor(core::float4(1, 1, 1, 1)),
        m_enableNormal(true),
        m_normalMap(nullptr),
        m_normalStrength(1.0f),
        m_enablePbr(true),
        m_pbrMap(nullptr),
        m_occlusion(1.0f),
        m_roughness(1.0f),
        m_metalness(1.0f),
        m_enableEmissive(true),
        m_emissiveMap(nullptr),
        m_emissiveColor(core::float4(1, 1, 1, 1)),
        m_emissiveIntensity(1.0f)
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
        // Common
        _data->setSurfaceType(m_surfaceType);
        _data->setCullMode(m_cullMode);
   
        // Alpha
        _data->setAlphaSource(m_alphaSource);
        _data->setDepthFade(m_depthFade);

        // Instance & vertex colors/alpha
        _data->setInstanceColorMask(m_instanceColorMask);
        _data->setVertexColorMask(m_vertexColorMask);

        // UV
        _data->setUVSource(m_UVSource);
        _data->setTiling(m_tiling);
        _data->setOffset(m_offset);

        // Albedo
        const bool enableAlbedo = (SurfaceType::Decal == m_surfaceType) | m_enableAlbedo;
        _data->setAlbedoTextureHandle(enableAlbedo && m_albedoMap ? m_albedoMap->getTextureHandle() : RESERVEDSLOT_TEXSRV_DEFAULT_ALBEDO);
        _data->setAlbedoColor(m_albedoColor);

        // Normal
        _data->setNormalTextureHandle(m_enableNormal && m_normalMap ? m_normalMap->getTextureHandle() : RESERVEDSLOT_TEXSRV_DEFAULT_NORMAL);
        _data->setNormalStrength(m_normalStrength);

        // PBR
        _data->setPBRTextureHandle(m_enablePbr && m_pbrMap ? m_pbrMap->getTextureHandle() : RESERVEDSLOT_TEXSRV_DEFAULT_PBR);
        _data->setOcclusion(m_occlusion);
        _data->setRoughness(m_roughness);
        _data->setMetalness(m_metalness);

        // Emissive
        _data->setEmissiveTextureHandle(m_enableEmissive && m_emissiveMap ? m_emissiveMap->getTextureHandle() : RESERVEDSLOT_TEXSRV_DEFAULT_EMISSIVE);
        _data->setEmissiveColor(m_emissiveColor);
        _data->setEmissiveIntensity(m_emissiveIntensity);
    }

    //--------------------------------------------------------------------------------------
    void DefaultMaterialModel::Setup(const RenderContext & _renderContext, CommandList * _cmdList, RootConstants3D * _root3D, core::uint _index) const
    {
        _root3D->setMatID(_index);

        auto key = m_shaderKey[asInteger(_renderContext.m_shaderPass)];
        VG_ASSERT(0xFF != key.file, "Undefined ShaderKey for pass \"%s\"", asString(_renderContext.m_shaderPass).c_str());

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

                key.setFlag(DefaultHLSLDesc::Toolmode, false);
                key.setFlag(DefaultHLSLDesc::ZOnly, true);
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

                _cmdList->setBlendState(bs);

                if (_renderContext.m_raytracing)
                    key.setFlag(DefaultHLSLDesc::RayTracing, true);

                if (_renderContext.m_toolmode)
                {
                    if (_renderContext.m_wireframe)
                    {
                        rs = RasterizerState(FillMode::Wireframe, CullMode::None);
                        _root3D->setFlags(RootConstantsFlags::Wireframe);
                    }

                    key.setFlag(DefaultHLSLDesc::Toolmode, true);
                }
                else
                {
                    key.setFlag(DefaultHLSLDesc::Toolmode, false);
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
                        bs.setColorWriteMask(0, (ColorWrite)0x0);
                        bs.enableIndependantBlend(true);
                    }
                }
                    
                _cmdList->setBlendState(bs);

                VG_ASSERT(!_renderContext.m_wireframe);

                if (_renderContext.m_toolmode)
                    key.setFlag(DefaultHLSLDesc::Toolmode, true);
                else
                    key.setFlag(DefaultHLSLDesc::Toolmode, false);
            }
            break;

            case ShaderPass::Outline:
            {
                BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);
                _cmdList->setBlendState(bs);

                key.setFlag(DefaultHLSLDesc::Toolmode, false);
            }
            break;
        }   

        if (_renderContext.m_particle)
        {
            key.setFlag(DefaultHLSLDesc::Particle, true);
        }

        CullMode cullMode;

        // During raytracing, cullmode has no effect on opaques because front/back faces cannot be manually tested.
        // We do not use the RAY_FLAG_CULL_FRONT_FACING_TRIANGLES flag during raytrace because:
        // 1) - Can be worse for performance on AMD (https://gpuopen.com/learn/improving-rt-perf-with-rra/)
        // 2) - When we do alphatest it's during the same 'query.Proceed()' loop and we want to be able to filter back/front faces
        
        //if (m_surfaceType == SurfaceType::Opaque)
        //{
        //    cullMode = CullMode::None;
        //}
        //else
        {
            switch (m_cullMode)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(m_cullMode);
                    break;
                case CullMode::None:
                    cullMode = CullMode::None;
                    break;
                case CullMode::Back:
                    cullMode = _renderContext.m_reverseCullMode ? CullMode::Front : CullMode::Back;
                    break;
                case CullMode::Front:
                    cullMode = _renderContext.m_reverseCullMode ? CullMode::Back : CullMode::Front;
                    break;

            }
        }
        rs = RasterizerState(rs.getFillMode(), cullMode);

        _cmdList->setRasterizerState(rs);

        key.setValue(DefaultHLSLDesc::Surface, m_surfaceType);

        _cmdList->setShader(key);
    }

    //--------------------------------------------------------------------------------------
    void DefaultMaterialModel::SetSurfaceType(SurfaceType _surfaceType)
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
    void DefaultMaterialModel::SetTexture(const core::string & _name, ITexture * _value)
    {
        if (_name == "m_albedoMap")
            assignTexture((Texture **)&m_albedoMap, (Texture *)_value);
        else if (_name == "m_normalMap")
            assignTexture((Texture **)&m_normalMap, (Texture *)_value);
        else if (_name == "m_pbrMap")
            assignTexture((Texture **)&m_pbrMap, (Texture *)_value);
        else if (_name == "m_emissiveMap")
            assignTexture((Texture **)&m_emissiveMap, (Texture *)_value);
        else
            VG_WARNING("[Material] Texture \"%s\" not found", _name.c_str());
    }

    //--------------------------------------------------------------------------------------
    void DefaultMaterialModel::assignTexture(Texture ** _texture, Texture * _value)
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
        //if (_name == "NormalStrength")
        //    m_normalStrength = _value;
        //else if (_name == "Occlusion")
        //    m_occlusion = _value;
        //else if (_name == "Roughness")
        //    m_roughness = _value;
        //else if (_name == "Metalness")
        //    m_metalness = _value;

        if (IProperty * prop = GetClassDesc()->GetPropertyByName(_name.c_str()))
        {
            float * dst = prop->GetPropertyFloat(this);
            *dst = _value;
        }
    }

    //--------------------------------------------------------------------------------------
    void DefaultMaterialModel::SetFloat2(const core::string & _name, core::float2 _value)
    {
        if (_name == "Tiling")
            m_tiling = _value;
    }
}