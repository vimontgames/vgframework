#pragma once

#include "graphics/driver/Shader/Shader_consts.h"
#include "graphics/driver/Resource/Texture_consts.h"

namespace vg::graphics::driver
{
    class RootSignatureTableDesc
    {
    public:
        class Descriptor
        {
        public:
            enum class Type
            {
                Texture = 0,
                Buffer,
                UAVTexture,
                UAVBuffer,
                ConstantBuffer,
                Sampler
            };

            Type getDescriptorType() const { return m_type; }

            enum Offset : core::u16
            {
                Append = (core::u16)-1
            };

            struct DescriptorParameters
            {
                DescriptorParameters(core::u8 _register, core::u32 _count, core::u8 _space, core::u16 _offset = Offset::Append) :
                    m_register(_register),
                    m_count(_count),
                    m_space(_space),
                    m_offset(_offset)
                {

                }

                core::u8 m_register;
                core::u8 m_space;
                core::u16 m_offset;
                core::u32 m_count;

                inline bool operator == (const DescriptorParameters & _other) const { return m_register == _other.m_register && m_count == _other.m_count && m_space == _other.m_space && m_offset == _other.m_offset; }
                inline bool operator != (const DescriptorParameters & _other) const { return !(operator == (_other)); }
            };

            struct Texture : public DescriptorParameters
            {
                Texture(TextureType _texType, core::u8 _register, core::u32 _count, core::u8 _space, core::u16 _offset = Offset::Append) :
                    DescriptorParameters(_register, _count, _space, _offset),
                    m_texType(_texType)
                {

                }

                TextureType m_texType;
            };

            struct Buffer : public DescriptorParameters
            {
                Buffer(core::u8 _register, core::u32 _count, core::u8 _space, core::u16 _offset = Offset::Append) :
                    DescriptorParameters(_register, _count, _space, _offset)
                {

                }
            };

            struct UAVTexture : public DescriptorParameters
            {
                UAVTexture(core::u8 _register, core::u32 _count, core::u8 _space, core::u16 _offset = Offset::Append) :
                    DescriptorParameters(_register, _count, _space, _offset)
                {

                }
            };

            struct UAVBuffer : public DescriptorParameters
            {
                UAVBuffer(core::u8 _register, core::u32 _count, core::u8 _space, core::u16 _offset = Offset::Append) :
                    DescriptorParameters(_register, _count, _space, _offset)
                {

                }
            };

            struct ConstantBuffer : public DescriptorParameters
            {
                ConstantBuffer(core::u8 _register, core::u32 _count, core::u8 _space, core::u16 _offset = Offset::Append) :
                    DescriptorParameters(_register, _count, _space, _offset)
                {

                }
            };

            struct Sampler : public DescriptorParameters
            {
                Sampler(core::u8 _register = 0, core::u32 _count = 1, core::u8 _space = 0, core::u16 _offset = Offset::Append) :
                    DescriptorParameters(_register, _count, _space, _offset)
                {

                }
            };

            bool operator == (const Descriptor & _other) const
            {
                if (m_type != _other.m_type)
                    return false;

                switch (m_type)
                {
                    default:VG_ASSERT(false);
                        return false;
                    case Type::Texture:
                        return texture == _other.texture;
                    case Type::Buffer:
                        return buffer == _other.buffer;
                    case Type::UAVTexture:
                        return uavTexture == _other.uavTexture;
                    case Type::UAVBuffer:
                        return uavBuffer == _other.uavBuffer;
                    case Type::ConstantBuffer:
                        return constantBuffer == _other.constantBuffer;
                    case Type::Sampler:
                        return sampler == _other.sampler;
                }

                return true;
            }
            inline bool operator != (const Descriptor & _other) const { return !(operator == (_other)); }

            Descriptor() : 
                m_type((Type)-1)
            {
            }

            const Texture        & getTextures         () const { VG_ASSERT(Type::Texture        == m_type); return texture; }
            const Buffer         & getBuffers          () const { VG_ASSERT(Type::Buffer         == m_type); return buffer; }
            const UAVTexture     & getUAVTextures      () const { VG_ASSERT(Type::UAVTexture     == m_type); return uavTexture; }
            const UAVBuffer      & getUAVBuffers       () const { VG_ASSERT(Type::UAVBuffer      == m_type); return uavBuffer; }
            const ConstantBuffer & getConstantBuffers  () const { VG_ASSERT(Type::ConstantBuffer == m_type); return constantBuffer; }
            const Sampler        & getSamplers         () const { VG_ASSERT(Type::Sampler        == m_type); return sampler; }

        private:
            Type m_type;

            union
            {
                Texture           texture;
                Buffer            buffer;
                UAVTexture        uavTexture;
                UAVBuffer         uavBuffer;
                ConstantBuffer    constantBuffer;
                Sampler           sampler;
            };

            friend class RootSignatureTableDesc;
        };

    public:
        void addTextures(TextureType _texType, core::u8 _register, core::u32 _count = 1, core::u8 _space = 0, core::u16 _offset = Descriptor::Offset::Append);
        void addBuffers(core::u8 _register, core::u32 _count = 1, core::u8 _space = 0, core::u16 _offset = Descriptor::Offset::Append);

        const core::vector<Descriptor> & getDescriptors() const;

        bool    empty() const;

        void setShaderStageFlags(ShaderStageFlags _stages);
        ShaderStageFlags getShaderStageFlags() const { return m_stages; }

        bool    operator == (const RootSignatureTableDesc & _other) const;
        inline bool operator != (const RootSignatureTableDesc & _other) const { return !(operator == (_other)); }

    private:
        ShaderStageFlags            m_stages = (ShaderStageFlags)0;
        core::vector<Descriptor>    m_descriptors;
    };
}