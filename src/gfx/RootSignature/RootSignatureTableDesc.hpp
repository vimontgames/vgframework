#include "RootSignatureTableDesc.h"

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    bool RootSignatureTableDesc::empty() const
    {
        return 0 == m_descriptors.size() || (ShaderStageFlags)0 == m_stages;
    }

    //--------------------------------------------------------------------------------------
    bool RootSignatureTableDesc::operator == (const RootSignatureTableDesc & _other) const
    {
        if (m_stages != _other.m_stages)
            return false;

        const uint descriptorCount = (uint)m_descriptors.size();
        if (descriptorCount != _other.m_descriptors.size())
            return false;

        for (uint i = 0; i < descriptorCount; ++i)
            if (m_descriptors[i] != _other.m_descriptors[i])
                return false;

        return true;
    }

    //--------------------------------------------------------------------------------------
    void RootSignatureTableDesc::setShaderStageFlags(ShaderStageFlags _stages)
    {
        m_stages = _stages;
    }

    //--------------------------------------------------------------------------------------
    void RootSignatureTableDesc::addConstantBuffers(core::u8 _binding, core::u16 _register, core::u16 _count)
    {
        RootSignatureTableDesc::Descriptor descritor;
        descritor.m_type = RootSignatureDescType::ConstantBuffer;
        descritor.constantBuffer = Descriptor::ConstantBuffer(_binding, _register, _count);
        m_descriptors.push_back(descritor);
    }

    //--------------------------------------------------------------------------------------
    void RootSignatureTableDesc::addTextures(core::u8 _binding, core::u16 _register, core::u16 _count)
    {
        RootSignatureTableDesc::Descriptor descritor;
        descritor.m_type = RootSignatureDescType::Texture;
        descritor.texture = Descriptor::Texture(_binding, _register, _count);
        m_descriptors.push_back(descritor);
    }

    //--------------------------------------------------------------------------------------
    void RootSignatureTableDesc::addBuffers(core::u8 _binding, core::u16 _register, core::u16 _count)
    {
        RootSignatureTableDesc::Descriptor descritor;
        descritor.m_type = RootSignatureDescType::Buffer;
        descritor.buffer = Descriptor::Buffer(_binding, _register, _count);
        m_descriptors.push_back(descritor);
    }

    //--------------------------------------------------------------------------------------
    void RootSignatureTableDesc::addRWTextures(core::u8 _binding, core::u16 _register, core::u16 _count)
    {
        RootSignatureTableDesc::Descriptor descritor;
        descritor.m_type = RootSignatureDescType::RWTexture;
        descritor.rwTexture = Descriptor::RWTexture(_binding, _register, _count);
        m_descriptors.push_back(descritor);
    }

    //--------------------------------------------------------------------------------------
    void RootSignatureTableDesc::addRWBuffers(core::u8 _binding, core::u16 _register, core::u16 _count)
    {
        RootSignatureTableDesc::Descriptor descritor;
        descritor.m_type = RootSignatureDescType::RWBuffer;
        descritor.rwBuffer = Descriptor::RWBuffer(_binding, _register, _count);
        m_descriptors.push_back(descritor);
    }

    //--------------------------------------------------------------------------------------
    void RootSignatureTableDesc::addTLAS(core::u8 _binding, core::u16 _register, core::u16 _count)
    {
        RootSignatureTableDesc::Descriptor descritor;
        descritor.m_type = RootSignatureDescType::TLAS;
        descritor.tlas = Descriptor::TLAS(_binding, _register, _count);
        m_descriptors.push_back(descritor);
    }

    //--------------------------------------------------------------------------------------
    const core::vector<RootSignatureTableDesc::Descriptor> & RootSignatureTableDesc::getDescriptors() const
    {
        return m_descriptors;
    }
}