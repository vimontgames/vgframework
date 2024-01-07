#include "DefaultMaterialModel.h"
#include "Shaders/default/default.hlsl.h"
#include "Shaders/system/instancedata.hlsli"

namespace vg::renderer
{
    using namespace vg::core;
    using namespace vg::gfx;

    VG_REGISTER_OBJECT_CLASS_EX(DefaultMaterialModel, "Default", IClassDesc::Flags::Model);

    //--------------------------------------------------------------------------------------
    bool DefaultMaterialModel::registerProperties(core::IClassDesc & _desc)
    {
        return super::registerProperties(_desc);
    }

    //--------------------------------------------------------------------------------------
    DefaultMaterialModel::DefaultMaterialModel(const core::string & _name, IObject * _parent) :
        MaterialModel(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    DefaultMaterialModel::~DefaultMaterialModel()
    {

    }

    //--------------------------------------------------------------------------------------
    void DefaultMaterialModel::FillGPUMaterialData(GPUMaterialData * _data) const
    {
        _data->setAlbedoTextureHandle(m_albedoMap ? m_albedoMap->getTextureHandle() : RESERVEDSLOT_TEXSRV_DEFAULT_ALBEDO);
        _data->setNormalTextureHandle(m_normaMap ? m_normaMap->getTextureHandle() : RESERVEDSLOT_TEXSRV_DEFAULT_NORMAL);
        _data->setPBRTextureHandle(m_pbrMap ? m_pbrMap->getTextureHandle() : RESERVEDSLOT_TEXSRV_DEFAULT_PBR);
    }

    //--------------------------------------------------------------------------------------
    void DefaultMaterialModel::Setup(const RenderContext & _renderContext, gfx::CommandList * _cmdList, RootConstants3D * _root3D, core::uint _index) const
    {
        gfx::Texture * albedoMap = m_albedoMap;
        gfx::Texture * normalMap = m_normaMap;
        gfx::Texture * pbrMap = m_pbrMap;
  
        //_root3D->setAlbedoTextureHandle( albedoMap ? albedoMap->getTextureHandle() : RESERVEDSLOT_TEXSRV_DEFAULT_ALBEDO ); 
        //_root3D->setNormalTextureHandle( normalMap ? normalMap->getTextureHandle() : RESERVEDSLOT_TEXSRV_DEFAULT_NORMAL);
        //_root3D->setPBRTextureHandle(normalMap ? normalMap->getTextureHandle() : RESERVEDSLOT_TEXSRV_DEFAULT_PBR);

        _root3D->setMatID(_index);
        _root3D->setColor(m_albedoColor);

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

                key.setFlags(gfx::DefaultHLSLDesc::Toolmode, false);
                key.setFlags(gfx::DefaultHLSLDesc::ZOnly, true);
            }
            break;

            case ShaderPass::Forward:
            case ShaderPass::Transparent:
            {
                BlendState bs;
                if (_renderContext.m_shaderPass == ShaderPass::Forward)
                    bs = BlendState(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);
                else
                    bs = BlendState(BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha, BlendOp::Add);
                _cmdList->setBlendState(bs);

                if (_renderContext.m_raytracing)
                    key.setFlags(gfx::DefaultHLSLDesc::RayTracing, true);

                if (_renderContext.m_toolmode)
                {
                    if (_renderContext.m_wireframe)
                    {
                        rs = RasterizerState(FillMode::Wireframe, CullMode::None);
                        _root3D->setFlags(RootConstantsFlags::Wireframe);
                    }

                    key.setFlags(gfx::DefaultHLSLDesc::Toolmode, true);
                }
                else
                {
                    key.setFlags(gfx::DefaultHLSLDesc::Toolmode, false);
                }
            }
            break;

            case ShaderPass::Deferred:
            {
                BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);
                _cmdList->setBlendState(bs);

                VG_ASSERT(!_renderContext.m_wireframe);

                if (_renderContext.m_toolmode)
                    key.setFlags(gfx::DefaultHLSLDesc::Toolmode, true);
                else
                    key.setFlags(gfx::DefaultHLSLDesc::Toolmode, false);
            }
            break;
        }        

        _cmdList->setRasterizerState(rs);

        if (_renderContext.m_alphatest)
            key.setFlags(gfx::DefaultHLSLDesc::AlphaTest);

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
        if (_name == "AlbedoMap")
            assignTexture((gfx::Texture**)&m_albedoMap, (gfx::Texture *)_value);
        else if (_name == "NormalMap")
            assignTexture((gfx::Texture**)&m_normaMap, (gfx::Texture *)_value);
        else if (_name == "PBRMap")
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
}