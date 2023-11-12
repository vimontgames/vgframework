#pragma once

#include "gfx/RootSignature/RootSignature_consts.h"
#include "gfx/Shader/ShaderKey.h"

#include "renderer/IMaterialModel.h"
#include "renderer/Model/Material/Material_Consts.h"

struct RootConstants3D;

namespace vg
{
    namespace gfx
    {
        class CommandList;
        class Texture;
        class Buffer;
    }

    namespace renderer
    {
        class RenderContext;
        class MaterialImporterData;

        enum class ShaderPass : core::u8
        {
            ZOnly = 0,
            Forward
        };

        class MaterialModel : public IMaterialModel
        {
            using super = IMaterialModel;

        public:

            enum Flags : core::u32
            {
                AlphaTest   = 0x00000001,
                AlphaBlend  = 0x00000002
            };

            const char *            getClassName            () const final { return "MaterialModel"; }

            static bool             registerClass           (core::IFactory & _factory);
            static bool             registerProperties      (core::IClassDesc & _desc);

                                    MaterialModel           (const core::string & _name, core::IObject * _parent = nullptr);
                                    ~MaterialModel          ();

            void                    Setup                   (const RenderContext & _renderContext, gfx::CommandList * _cmdList, RootConstants3D * _root3DConstants, core::uint _index) const;

            core::uint              GetTextureCount         () const override;
            gfx::ITexture *         GetTexture              (MaterialTextureType _type) const override;
            const core::string &    GetTexturePath          (MaterialTextureType _type) const override;

            void                    SetTexture              (MaterialTextureType _type, gfx::ITexture * _texture) final;

            core::uint              getTextureCount         () const { return (core::uint)core::countof(m_textureInfos); }
            gfx::Texture *          getTexture              (MaterialTextureType _type) const;
            const core::string &    getTexturePath          (MaterialTextureType _type) const { return m_textureInfos[core::asInteger(_type)].path; };

            static MaterialModel *  createFromImporterData  (const MaterialImporterData & _data);

        private:

            struct TextureInfo
            {
                core::string    path;
                gfx::Texture *  texture = nullptr;
            };

            Flags                       m_flags;
            TextureInfo                 m_textureInfos[core::enumCount<MaterialTextureType>()];
            //core::float4                m_colors[core::enumCount<MaterialColorType>()];
            gfx::RootSignatureHandle    m_rootSignature;
            gfx::ShaderKey              m_shaderKey[core::enumCount<ShaderPass>()];
        };
    }
}