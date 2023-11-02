#pragma once

#include "gfx/Shader/Shader_consts.h"
#include "gfx/Resource/Texture_consts.h"

namespace vg::gfx
{
    class RootSignatureTableDesc
    {
    public:
        class Descriptor
        {
        public:
            enum class Type
            {
                ConstantBuffer = 0,
                Texture,
                Buffer,
                RWTexture,
                RWBuffer,
                Sampler
            };

            Type getDescriptorType() const { return m_type; }

            struct DescriptorParameters
            {
                DescriptorParameters(core::u8 _binding, core::u16 _register, core::u16 _count) :
                    m_binding(_binding),
                    m_register(_register),
                    m_count(_count)
                {

                }

                core::u8 m_binding;
                core::u16 m_register;
                core::u16 m_count;

                inline bool operator == (const DescriptorParameters & _other) const { return m_binding == _other.m_binding && m_register == _other.m_register && m_count == _other.m_count; }
                inline bool operator != (const DescriptorParameters & _other) const { return !(operator == (_other)); }
            };

            struct Texture : public DescriptorParameters
            {
                Texture(core::u8 _binding, core::u16 _register, core::u16 _count) :
                    DescriptorParameters(_binding,_register, _count)
                {

                }
            };

            struct Buffer : public DescriptorParameters
            {
                Buffer(core::u8 _binding, core::u16 _register, core::u16 _count) :
                    DescriptorParameters(_binding, _register, _count)
                {

                }
            };

            struct RWTexture : public DescriptorParameters
            {
                RWTexture(core::u8 _binding, core::u16 _register, core::u16 _count) :
                    DescriptorParameters(_binding, _register, _count)
                {

                }
            };

            struct RWBuffer : public DescriptorParameters
            {
                RWBuffer(core::u8 _binding, core::u16 _register, core::u16 _count) :
                    DescriptorParameters(_binding, _register, _count)
                {

                }
            };

            struct ConstantBuffer : public DescriptorParameters
            {
                ConstantBuffer(core::u8 _binding, core::u16 _register, core::u16 _count) :
                    DescriptorParameters(_binding, _register, _count)
                {

                }
            };

            struct Sampler : public DescriptorParameters
            {
                Sampler(core::u8 _binding, core::u16 _register, core::u16 _count) :
                    DescriptorParameters(_binding, _register, _count)
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
                    case Type::RWTexture:
                        return rwTexture == _other.rwTexture;
                    case Type::RWBuffer:
                        return rwBuffer == _other.rwBuffer;
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

            const Texture &         getTextures         () const { VG_ASSERT(Type::Texture        == m_type); return texture; }
            const Buffer &          getBuffers          () const { VG_ASSERT(Type::Buffer         == m_type); return buffer; }
            const RWTexture &       getRWTextures       () const { VG_ASSERT(Type::RWTexture      == m_type); return rwTexture; }
            const RWBuffer &        getRWBuffers        () const { VG_ASSERT(Type::RWBuffer       == m_type); return rwBuffer; }
            const ConstantBuffer &  getConstantBuffers  () const { VG_ASSERT(Type::ConstantBuffer == m_type); return constantBuffer; }
            const Sampler &         getSamplers         () const { VG_ASSERT(Type::Sampler        == m_type); return sampler; }

        private:
            Type m_type;

            union
            {
                ConstantBuffer  constantBuffer;
                Texture         texture;
                Buffer          buffer;
                RWTexture       rwTexture;
                RWBuffer        rwBuffer;
                Sampler         sampler;
            };

            friend class RootSignatureTableDesc;
        };

    public:
        void                                addConstantBuffers  (core::u8 _binding, core::u16 _register, core::u16 _count);
        void                                addTextures         (core::u8 _binding, core::u16 _register, core::u16 _count);
        void                                addBuffers          (core::u8 _binding, core::u16 _register, core::u16 _count);

        void                                addRWTextures       (core::u8 _binding, core::u16 _register, core::u16 _count);
        void                                addRWBuffers        (core::u8 _binding, core::u16 _register, core::u16 _count);

        const core::vector<Descriptor> &    getDescriptors      () const;

        bool                                empty               () const;

        void                                setShaderStageFlags (ShaderStageFlags _stages);
        ShaderStageFlags                    getShaderStageFlags () const { return m_stages; }

        bool                                operator ==         (const RootSignatureTableDesc & _other) const;
        inline bool                         operator !=         (const RootSignatureTableDesc & _other) const { return !(operator == (_other)); }

    private:
        ShaderStageFlags            m_stages = (ShaderStageFlags)0;
        core::vector<Descriptor>    m_descriptors;
    };
}