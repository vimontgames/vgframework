#include "DefaultMaterialModel.h"
#include "Shaders/default/default.hlsl.h"

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
    void DefaultMaterialModel::Setup(const RenderContext & _renderContext, gfx::CommandList * _cmdList, RootConstants3D * _root3D, core::uint _index) const
    {
        gfx::Texture * albedoMap = m_albedoMap;
        gfx::Texture * normalMap = m_normaMap;

        const Renderer * renderer = Renderer::get();
  
        _root3D->setAlbedoTextureHandle( albedoMap ? albedoMap->getTextureHandle() : renderer->getDefaultTexture(MaterialTextureType::Albedo)->getTextureHandle());
        _root3D->setNormalTextureHandle( normalMap ? normalMap->getTextureHandle() : renderer->getDefaultTexture(MaterialTextureType::Normal)->getTextureHandle());
        _root3D->setMatID(_index);
        _root3D->setColor(m_albedoColor);

        auto key = m_shaderKey[asInteger(_renderContext.m_shaderPass)];

        RasterizerState rs(FillMode::Solid, CullMode::Back);

        _cmdList->setGraphicRootSignature(m_rootSignature);
        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleList);

        //DepthStencilState ds(true, true, ComparisonFunc::LessEqual);
        //_cmdList->setDepthStencilState(ds);

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
            {
                BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);
                _cmdList->setBlendState(bs);

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
        }

        

        _cmdList->setRasterizerState(rs);
        _cmdList->setShader(key);
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
}