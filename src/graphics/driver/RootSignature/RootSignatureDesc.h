#pragma once

#include "Graphics/driver/Shader/Shader_consts.h"

namespace vg::graphics::driver
{
    class RootSignatureDesc
    {
    public:
        struct PushConstantParams
        {
            inline bool operator == (const PushConstantParams & _other) const
            {
                return m_stages == _other.m_stages
                    && m_start  == _other.m_start
                    && m_count  == _other.m_count
                    && m_space  == _other.m_space; 
            }
            inline bool operator != (const PushConstantParams & _other) const { return !(operator == (_other)); }

            ShaderStageFlags    m_stages = (ShaderStageFlags)0;
            core::u8            m_start = 0;
            core::u8            m_count = 0;
            core::u8            m_space = 0;
        };

        class Table
        {
            class Descriptor
            {
            public:
                enum class Type
                {
                    ShaderResource = 0,
                    RWBuffer,
                    ConstantBuffer,
                    Sampler
                };

                Type getDescriptorType() const { return m_type; }

                struct ConstantBufferParams
                {
                    core::u8 m_start;
                    core::u8 m_count;
                    core::u8 m_space;

                    inline bool operator == (const ConstantBufferParams & _other) const
                    {
                        return m_start == _other.m_start && m_count == _other.m_count && m_space == _other.m_space;
                    }
                    inline bool operator != (const ConstantBufferParams & _other) const { return !(operator == (_other)); }
                };
                struct BufferParams
                {

                    inline bool operator == (const ConstantBufferParams & _other) const 
                    { 
                        return false; 
                    }
                    inline bool operator != (const ConstantBufferParams & _other) const { return !(operator == (_other)); }
                };
                struct TextureParams
                {
                    inline bool operator == (const TextureParams & _other) const
                    {
                        return false;
                    }
                    inline bool operator != (const TextureParams & _other) const { return !(operator == (_other)); }
                };

                const ConstantBufferParams & getConstantBufferParams() const { VG_ASSERT(Type::ConstantBuffer == m_type); return constantBuffer; }

                bool operator == (const Descriptor & _other) const
                {
                    if (m_type != _other.m_type)
                        return false;

                    switch (m_type)
                    {
                        default:VG_ASSERT(false);   return false;
                        case Type::ShaderResource:  return constantBuffer == _other.constantBuffer;
                        case Type::RWBuffer:        return constantBuffer == _other.constantBuffer;
                        case Type::ConstantBuffer:  return constantBuffer == _other.constantBuffer;
                        case Type::Sampler:         return constantBuffer == _other.constantBuffer;
                    }

                    return true;
                }
                inline bool operator != (const Descriptor & _other) const { return !(operator == (_other)); }

            private:
                Type m_type;

                union
                {
                    ConstantBufferParams constantBuffer;
                    BufferParams buffer;
                    TextureParams texture;
                };
            };

        public:

            void    addConstantBuffer();
            void    addBuffer();
            void    addTexture();

            const core::vector<Descriptor> & getDescriptors() const;

            bool    isEmpty() const;
            bool    operator == (const Table & _other) const;
            inline bool operator != (const Table & _other) const { return !(operator == (_other)); }

        private:
            ShaderStageFlags            m_stages = (ShaderStageFlags)0;
            core::vector<Descriptor>    m_descriptors;
        };

        core::uint addRootConstants(ShaderStageFlags _stages, core::u8 _offset, core::u8 _count);
        core::uint addTable(const Table & _table);

        const core::vector<PushConstantParams> & getRootConstants() const { return m_pushConstants; }
        const core::vector<Table> & getTables() const { return m_tables; }

        bool operator == (const RootSignatureDesc & _other) const;
        inline bool operator != (const RootSignatureDesc & _other) const {  return !(operator == (_other)); }

    private:
        core::vector<PushConstantParams>    m_pushConstants;
        core::vector<Table>                 m_tables;
    };
}