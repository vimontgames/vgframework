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
    void RootSignatureTableDesc::addConstantBuffers(core::u8 _register, core::u32 _count, core::u8 _space, core::u16 _offset)
    {
        RootSignatureTableDesc::Descriptor descritor;
        descritor.m_type = Descriptor::Type::ConstantBuffer;
        descritor.constantBuffer = Descriptor::ConstantBuffer(_register, _count, _space, _offset);
        m_descriptors.push_back(descritor);
    }

    //--------------------------------------------------------------------------------------
    void RootSignatureTableDesc::addTextures(core::u8 _register, core::u32 _count, core::u8 _space, core::u16 _offset)
    {
        RootSignatureTableDesc::Descriptor descritor;
        descritor.m_type = Descriptor::Type::Texture;
        descritor.texture = Descriptor::Texture(_register, _count, _space, _offset);
        m_descriptors.push_back(descritor);
    }

    //--------------------------------------------------------------------------------------
    void RootSignatureTableDesc::addBuffers(core::u8 _register, core::u32 _count, core::u8 _space, core::u16 _offset)
    {
        RootSignatureTableDesc::Descriptor descritor;
        descritor.m_type = Descriptor::Type::Buffer;
        descritor.buffer = Descriptor::Buffer(_register, _count, _space, _offset);
        m_descriptors.push_back(descritor);
    }

    //--------------------------------------------------------------------------------------
    void RootSignatureTableDesc::addUAVTextures(core::u8 _register, core::u32 _count, core::u8 _space, core::u16 _offset)
    {
        RootSignatureTableDesc::Descriptor descritor;
        descritor.m_type = Descriptor::Type::UAVTexture;
        descritor.uavTexture = Descriptor::UAVTexture(_register, _count, _space, _offset);
        m_descriptors.push_back(descritor);
    }

    //--------------------------------------------------------------------------------------
    void RootSignatureTableDesc::addUAVBuffers(core::u8 _register, core::u32 _count, core::u8 _space, core::u16 _offset)
    {
        RootSignatureTableDesc::Descriptor descritor;
        descritor.m_type = Descriptor::Type::UAVBuffer;
        descritor.uavBuffer = Descriptor::UAVBuffer(_register, _count, _space, _offset);
        m_descriptors.push_back(descritor);
    }

    //--------------------------------------------------------------------------------------
    const core::vector<RootSignatureTableDesc::Descriptor> & RootSignatureTableDesc::getDescriptors() const
    {
        return m_descriptors;
    }
}