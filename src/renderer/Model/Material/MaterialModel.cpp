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

        registerPropertyEnum(MaterialModel, SurfaceType, m_surfaceType, "Surface Type");
        registerProperty(MaterialModel, m_depthFade, "Depth transparency");
        registerPropertyEnum(MaterialModel, CullMode, m_cullMode, "Cull");

        return true;
    }

    //--------------------------------------------------------------------------------------
    MaterialModel::MaterialModel(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent),
        m_surfaceType(SurfaceType::Opaque),
        m_cullMode(CullMode::Back)
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
        m_shaderKey[asInteger(ShaderPass::Deferred)] = ShaderKey(shaderFile, "Deferred");
        m_shaderKey[asInteger(ShaderPass::Transparent)] = ShaderKey(shaderFile, "Forward");
    }

    //--------------------------------------------------------------------------------------
    MaterialModel::~MaterialModel()
    {
        auto * device = Device::get();
        device->removeRootSignature(m_rootSignature);
    }
}