#pragma once

#include "Graphics/driver/Shader/Shader_consts.h"

namespace vg::graphics::driver
{
    class RootSignatureDesc
    {
    public:
        struct Constant
        {
            ShaderStageFlags    m_stages = (ShaderStageFlags)0;
            core::u8            m_offset = 0;
            core::u8            m_count = 0;
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

            private:
                Type m_type;

                union
                {
                    struct ConstantBufferParams
                    {

                    };
                    struct BufferParams
                    {

                    };
                    struct TextureParams
                    {

                    };
                };
            };

        public:

            void    addConstantBuffer();
            void    addBuffer();
            void    addTexture();

            bool    isEmpty() const;

        private:
            ShaderStageFlags            m_stages = (ShaderStageFlags)0;
            core::vector<Descriptor>    m_descriptors;
        };

        core::uint addTable(const Table & _table);
        core::uint addRootConstants(ShaderStageFlags _stages, core::u8 _offset, core::u8 _count);

    private:
        core::vector<Constant>  m_constants;
        core::vector<Table>     m_tables;
    };
}