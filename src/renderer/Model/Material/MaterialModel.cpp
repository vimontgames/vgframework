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

#include "shaders/system/rootConstants3D.hlsli"
#include "DefaultMaterial/DefaultMaterialModel.hpp"


using namespace vg::core;
using namespace vg::gfx;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    bool MaterialModel::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEnumBitfield(MaterialModel, MaterialFlags, m_flags, "Flags");
        //registerPropertyEnumArray(MaterialModel, core::float4, MaterialColorType, m_colors, "Colors", IProperty::Flags::Color | IProperty::Flags::NotSaved);
        //registerPropertyEnumArray(MaterialModel, core::IObject*, MaterialTextureType, m_textures, "Textures", IProperty::Flags::None | IProperty::Flags::NotSaved);

        return true;
    }

    //--------------------------------------------------------------------------------------
    MaterialModel::MaterialModel(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent),
        m_flags((MaterialFlags)0x0)
    {
        auto * device = Device::get();

        const RootSignatureTableDesc & bindlessTable = device->getBindlessTable()->getTableDesc();

        RootSignatureDesc rsDesc;
                          rsDesc.addRootConstants(ShaderStageFlags::All, 0, 0, RootConstants3DCount);
                          rsDesc.addTable(bindlessTable);

        m_rootSignature = device->addRootSignature(rsDesc);

        const char * shaderFile = "default/default.hlsl";
        m_shaderKey[asInteger(ShaderPass::ZOnly)] = ShaderKey(shaderFile, "ZOnly");
        m_shaderKey[asInteger(ShaderPass::Forward)] = ShaderKey(shaderFile, "Forward");
    }

    //--------------------------------------------------------------------------------------
    MaterialModel::~MaterialModel()
    {
        auto * device = Device::get();
        device->removeRootSignature(m_rootSignature);
    }
}