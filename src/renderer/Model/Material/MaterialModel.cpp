#include "renderer/Precomp.h"
#include "MaterialModel.h"

#include "core/Object/AutoRegisterClass.h"
#include "core/Object/EnumHelper.h"

#include "gfx/Device/Device.h"
#include "gfx/CommandList/CommandList.h"
#include "gfx/BindlessTable/BindlessTable.h"
#include "gfx/Resource/Texture.h"
#include "gfx/PipelineState/Graphic/RasterizerState.h"
#include "gfx/PipelineState/Graphic/DepthStencilState.h"
#include "gfx/PipelineState/Graphic/BlendState.h"
#include "renderer/RenderPass/RenderContext.h"
#include "renderer/Importer/SceneImporterData.h"
#include "renderer/Renderer.h"

#include "DefaultMaterial/DefaultMaterialModel.hpp"

#include "Shaders/default/default.hlsl.h"
#include "shaders/system/rootConstants3D.hlsli"

using namespace vg::core;
using namespace vg::gfx;

namespace vg::renderer
{
    VG_AUTO_REGISTER_CLASS(MaterialModel);

    //--------------------------------------------------------------------------------------
    bool MaterialModel::registerClass(core::IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassHelper(MaterialModel, "Material Model", IClassDesc::Flags::Model))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MaterialModel::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerPropertyEnumBitfield(MaterialModel, Flags, m_flags, "Flags");

        return true;
    }

    //--------------------------------------------------------------------------------------
    MaterialModel::MaterialModel(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent),
        m_flags((Flags)0x0)
    {
        auto * device = Device::get();

        const RootSignatureTableDesc & bindlessTable = device->getBindlessTable()->getTableDesc();

        RootSignatureDesc rsDesc;
                          rsDesc.addRootConstants(ShaderStageFlags::All, 0, 0, RootConstants3DCount);
                          rsDesc.addTable(bindlessTable);

        m_rootSignature = device->addRootSignature(rsDesc);
        m_shaderKey = ShaderKey("default/default.hlsl", "Forward");
    }

    //--------------------------------------------------------------------------------------
    MaterialModel::~MaterialModel()
    {
        auto * device = Device::get();
        device->removeRootSignature(m_rootSignature);

        for (uint t = 0; t < core::enumCount<MaterialTextureType>(); ++t)
            VG_SAFE_RELEASE(m_textureInfos[t].texture);
    }

    //--------------------------------------------------------------------------------------
    MaterialModel * MaterialModel::createFromImporterData(const MaterialImporterData & _data)
    {
        MaterialModel * matModel = new MaterialModel(_data.name);

        for (uint t = 0; t < core::enumCount<MaterialTextureType>(); ++t)
            matModel->m_textureInfos[t].path = _data.texturePath[t];

        return matModel;
    }

    //--------------------------------------------------------------------------------------
    gfx::ITexture * MaterialModel::GetTexture(MaterialTextureType _type) const
    {
        VG_ASSERT(asInteger(_type) < countof(m_textureInfos));
        return getTexture(_type);
    }

    //--------------------------------------------------------------------------------------
    gfx::Texture * MaterialModel::getTexture(MaterialTextureType _type) const
    { 
        auto * tex = m_textureInfos[asInteger(_type)].texture;
        if (tex)
            return tex;
        else
            return Renderer::get()->getDefaultTexture(_type);
    }

    //--------------------------------------------------------------------------------------
    core::uint MaterialModel::GetTextureCount() const
    {
        return getTextureCount();
    }

    //--------------------------------------------------------------------------------------
    const core::string & MaterialModel::GetTexturePath(MaterialTextureType _type) const
    {
        VG_ASSERT(asInteger(_type) < countof(m_textureInfos));
        return getTexturePath(_type);
    }

    //--------------------------------------------------------------------------------------
    void MaterialModel::SetTexture(MaterialTextureType _type, gfx::ITexture * _texture)
    {
        auto & tex = m_textureInfos[asInteger(_type)].texture;

        if (tex != _texture)
        {
            VG_SAFE_INCREASE_REFCOUNT(_texture);
            VG_SAFE_RELEASE(tex);
            tex = (gfx::Texture*)_texture;
        }
    }

    //--------------------------------------------------------------------------------------
    void MaterialModel::Setup(const RenderContext & _renderContext, gfx::CommandList * _cmdList, RootConstants3D * _root3D, core::uint _index) const
    {
        gfx::Texture * albedoMap = getTexture(MaterialTextureType::Albedo);
        gfx::Texture * normalMap = getTexture(MaterialTextureType::Normal);

        _root3D->setAlbedoTextureHandle(albedoMap->getTextureHandle());
        _root3D->setNormalTextureHandle(normalMap->getTextureHandle());
        _root3D->setMatID(_index);

        auto key = m_shaderKey;

        RasterizerState rs(FillMode::Solid, CullMode::Back);
        BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);
        DepthStencilState ds(true, true, ComparisonFunc::LessEqual);

        _cmdList->setGraphicRootSignature(m_rootSignature);
        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleList);
        _cmdList->setBlendState(bs);
        _cmdList->setDepthStencilState(ds);        

        // TODO: set from material?
        if (_renderContext.m_toolmode)
        {
            if (_renderContext.m_wireframe)
                rs = RasterizerState(FillMode::Wireframe, CullMode::None);

            key.setFlags(gfx::DefaultHLSLDesc::Toolmode, true);
        }
        else
        {
            key.setFlags(gfx::DefaultHLSLDesc::Toolmode, false);
        }
       
        _cmdList->setRasterizerState(rs);
        _cmdList->setShader(key);
    }
}