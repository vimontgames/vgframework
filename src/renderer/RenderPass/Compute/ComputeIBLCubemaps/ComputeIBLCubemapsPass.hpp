#include "ComputeIBLCubemapsPass.h"
#include "shaders/lighting/PrecomputeIBL.hlsli"

using namespace vg::gfx;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    ComputeIBLCubemapsPass::ComputeIBLCubemapsPass() :
        ComputePass("ComputeIrradianceCubemapPass")
    {
        auto * device = Device::get();

        const RootSignatureTableDesc & bindlessTable = device->getBindlessTable()->getTableDesc();

        RootSignatureDesc rsDesc;
        rsDesc.addRootConstants(ShaderStageFlags::CS, 0, 0, PrecomputeIBLConstantsCount);
        rsDesc.addTable(bindlessTable);

        m_computeIrradianceCubemapRootSignature = device->addRootSignature(rsDesc);
        m_computeIrradianceCubemapShaderKey.init("lighting/PrecomputeIBL.hlsl", "GenerateIrradianceCubemap");
        m_computeSpecularReflectionShaderKey.init("lighting/PrecomputeIBL.hlsl", "GenerateSpecularReflectionCubemap");
    }

    //--------------------------------------------------------------------------------------
    ComputeIBLCubemapsPass::~ComputeIBLCubemapsPass()
    {
        auto * device = Device::get();
        device->removeRootSignature(m_computeIrradianceCubemapRootSignature);
    }

    //--------------------------------------------------------------------------------------
    void ComputeIBLCubemapsPass::add(gfx::Texture * _sourceEnvironmentCubemap, gfx::Texture * _destinationIrradianceCubemap, gfx::Texture * _destinationSpecularReflectionCubemap)
    {
        if (_sourceEnvironmentCubemap && (_destinationIrradianceCubemap || _destinationSpecularReflectionCubemap))
            m_computeIBLCubemaps.push_back({ _sourceEnvironmentCubemap, _destinationIrradianceCubemap, _destinationSpecularReflectionCubemap });
    }
    
    //--------------------------------------------------------------------------------------
    void ComputeIBLCubemapsPass::Setup(const RenderPassContext & _renderPassContext)
    {

    }

    //--------------------------------------------------------------------------------------
    void ComputeIBLCubemapsPass::Render(const RenderPassContext & _renderPassContext, CommandList * _cmdList) const
    {
        for (uint i = 0; i < m_computeIBLCubemaps.size(); ++i)
        {
            const auto & data = m_computeIBLCubemaps[i];

            if (data.m_sourceEnvironmentCubemap)
            {
                if (data.m_destinationIrradianceCubemap)
                    computeIrradianceCubemap(_cmdList, data.m_sourceEnvironmentCubemap, data.m_destinationIrradianceCubemap);

                if (data.m_destinationSpecularReflectionCubemap)
                    computeSpecularReflectionCubemap(_cmdList, data.m_sourceEnvironmentCubemap, data.m_destinationSpecularReflectionCubemap);
            }
        }

        m_computeIBLCubemaps.clear();
    }

    //--------------------------------------------------------------------------------------
    void ComputeIBLCubemapsPass::computeIrradianceCubemap(CommandList * _cmdList, const gfx::Texture * _sourceEnvironmentCubemap, gfx::Texture * _destinationIrradianceCubemap) const
    {
        VG_PROFILE_GPU("IrradianceCubemap");

        const auto & srcTexDesc = _sourceEnvironmentCubemap->getTexDesc();
        const auto & dstTexDesc = _destinationIrradianceCubemap->getTexDesc();
        auto threadGroupSize = uint3(PRECOMPUTE_IBL_THREADGROUP_SIZE_X, PRECOMPUTE_IBL_THREADGROUP_SIZE_Y, PRECOMPUTE_IBL_THREADGROUP_SIZE_Z);
        auto threadGroupCount = uint3((dstTexDesc.width + threadGroupSize.x - 1) / threadGroupSize.x, (dstTexDesc.height + threadGroupSize.y - 1) / threadGroupSize.y, (6 + threadGroupSize.z - 1) / threadGroupSize.z);
        
        _cmdList->setComputeRootSignature(m_computeIrradianceCubemapRootSignature);
        _cmdList->setComputeShader(m_computeIrradianceCubemapShaderKey);

        // precompute the sample mip level so that source size = destination size
        const uint mipLevel = (uint)log(max(srcTexDesc.width / dstTexDesc.width, 1)) + 1;
        
        PrecomputeIBLConstants precomputeIBLConstants;
        {
            precomputeIBLConstants.setSource(_sourceEnvironmentCubemap->getTextureHandle());
            precomputeIBLConstants.setSourceMipLevel(mipLevel);
            precomputeIBLConstants.setSourceMipLevelCount(srcTexDesc.mipmaps);

            precomputeIBLConstants.setDestination(_destinationIrradianceCubemap->getRWTextureHandle());
            precomputeIBLConstants.setDestinationMipLevel(1);
            precomputeIBLConstants.setDestinationMipLevelCount(dstTexDesc.mipmaps);
        }
        _cmdList->setComputeRootConstants(0, (u32 *)&precomputeIBLConstants, PrecomputeIBLConstantsCount);
        
        _cmdList->dispatch(threadGroupCount);
        
        _cmdList->transitionResource(_destinationIrradianceCubemap, ResourceState::UnorderedAccess, ResourceState::ShaderResource);
    }

    //--------------------------------------------------------------------------------------
    void ComputeIBLCubemapsPass::computeSpecularReflectionCubemap(gfx::CommandList * _cmdList, const gfx::Texture * _sourceEnvironmentCubemap, gfx::Texture * _destinationSpecularReflectionCubemap) const
    {
        VG_PROFILE_GPU("SpecularReflectionCubemap");

        const auto & srcTexDesc = _sourceEnvironmentCubemap->getTexDesc();
        const auto & dstTexDesc = _destinationSpecularReflectionCubemap->getTexDesc();
        auto threadGroupSize = uint3(PRECOMPUTE_IBL_THREADGROUP_SIZE_X, PRECOMPUTE_IBL_THREADGROUP_SIZE_Y, PRECOMPUTE_IBL_THREADGROUP_SIZE_Z);
        
        _cmdList->setComputeRootSignature(m_computeIrradianceCubemapRootSignature);
        _cmdList->setComputeShader(m_computeSpecularReflectionShaderKey);

        uint width = dstTexDesc.width;
        uint height = dstTexDesc.height;

        for (uint m = 0; m < dstTexDesc.mipmaps; ++m)
        {
            auto threadGroupCount = uint3((width + threadGroupSize.x - 1) / threadGroupSize.x, (height + threadGroupSize.y - 1) / threadGroupSize.y, (6 + threadGroupSize.z - 1) / threadGroupSize.z);

            PrecomputeIBLConstants precomputeIBLConstants;
            {
                precomputeIBLConstants.setSource(_sourceEnvironmentCubemap->getTextureHandle());
                precomputeIBLConstants.setSourceMipLevel(0);
                precomputeIBLConstants.setSourceMipLevelCount(srcTexDesc.mipmaps);

                precomputeIBLConstants.setDestination(_destinationSpecularReflectionCubemap->getRWTextureHandle(m));
                precomputeIBLConstants.setDestinationMipLevel(m);
                precomputeIBLConstants.setDestinationMipLevelCount(dstTexDesc.mipmaps);
            }
            _cmdList->setComputeRootConstants(0, (u32 *)&precomputeIBLConstants, PrecomputeIBLConstantsCount);

            _cmdList->dispatch(threadGroupCount);

            width >>= 1;
            height >>= 1;
        }

        _cmdList->transitionResource(_destinationSpecularReflectionCubemap, ResourceState::UnorderedAccess, ResourceState::ShaderResource);
    }
}