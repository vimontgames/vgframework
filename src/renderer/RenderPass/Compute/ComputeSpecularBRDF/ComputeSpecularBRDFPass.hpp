#include "ComputeSpecularBRDFPass.h"
#include "shaders/lighting/PrecomputeIBL.hlsli"

using namespace vg::gfx;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    ComputeSpecularBRDFPass::ComputeSpecularBRDFPass() :
        ComputePass("ComputeSpecularBRDFPass")
    {
        auto * device = Device::get();

        const RootSignatureTableDesc & bindlessTable = device->getBindlessTable()->getTableDesc();

        RootSignatureDesc rsDesc;
        rsDesc.addRootConstants(ShaderStageFlags::CS, 0, 0, PrecomputeIBLConstantsCount);
        rsDesc.addTable(bindlessTable);

        m_computeSpecularBRDFRootSignature = device->addRootSignature(rsDesc);
        m_computeSpecularBRDFShaderKey.init("lighting/PrecomputeIBL.hlsl", "GenerateSpecularBRDF");
    }

    //--------------------------------------------------------------------------------------
    ComputeSpecularBRDFPass::~ComputeSpecularBRDFPass()
    {
        VG_SAFE_RELEASE_ASYNC(m_specularBRDFTexture);

        auto * device = Device::get();
        device->removeRootSignature(m_computeSpecularBRDFRootSignature);
    }

    //--------------------------------------------------------------------------------------
    void ComputeSpecularBRDFPass::setSpecularBRDFTexture(gfx::Texture * _texture)
    {
        if (_texture != m_specularBRDFTexture)
        {
            VG_SAFE_RELEASE_ASYNC(m_specularBRDFTexture);
            VG_SAFE_INCREASE_REFCOUNT(_texture);
            m_specularBRDFTexture = _texture;
        }
    }

    //--------------------------------------------------------------------------------------
    void ComputeSpecularBRDFPass::Setup(const RenderPassContext & _renderPassContext)
    {
        
    }

    //--------------------------------------------------------------------------------------
    void ComputeSpecularBRDFPass::Render(const RenderPassContext & _renderPassContext, CommandList * _cmdList) const
    {
        VG_ASSERT(m_specularBRDFTexture);
        if (nullptr != m_specularBRDFTexture)
        {
            const auto & texDesc = m_specularBRDFTexture->getTexDesc();
            auto threadGroupSize = uint2(PRECOMPUTE_IBL_THREADGROUP_SIZE_X, PRECOMPUTE_IBL_THREADGROUP_SIZE_Y);
            auto threadGroupCount = uint3((texDesc.width + threadGroupSize.x - 1) / threadGroupSize.x, (texDesc.height + threadGroupSize.y - 1) / threadGroupSize.y, 1);

            _cmdList->setComputeRootSignature(m_computeSpecularBRDFRootSignature);
            _cmdList->setComputeShader(m_computeSpecularBRDFShaderKey);

            PrecomputeIBLConstants precomputeIBLConstants;
            {
                precomputeIBLConstants.setDestination(m_specularBRDFTexture->getRWTextureHandle());
            }
            _cmdList->setComputeRootConstants(0, (u32 *)&precomputeIBLConstants, PrecomputeIBLConstantsCount);

            _cmdList->dispatch(threadGroupCount);

            // texture will be read-only from now on
            _cmdList->transitionResource(m_specularBRDFTexture, ResourceState::UnorderedAccess, ResourceState::ShaderResource);
        }
    }
}