#pragma once

#include "renderer/IMaterialModel.h"
#include "renderer/Model/Material/MaterialTextureType.h"

namespace vg
{
    namespace gfx
    {
        class Texture;
    }

    namespace renderer
    {
        class MaterialImporterData;

        class MaterialModel : public IMaterialModel
        {
            using super = IMaterialModel;

        public:

            enum Flags : core::u32
            {
                AlphaTest = 0x00000001,
                AlphaBlend = 0x00000002
            };

            const char *            getClassName            () const final { return "MaterialModel"; }

            static bool             registerClass           (core::IFactory & _factory);
            static bool             registerProperties      (core::IClassDesc & _desc);

                                    MaterialModel           (const core::string & _name, core::IObject * _parent = nullptr);
                                    ~MaterialModel          ();

            core::uint              GetTextureCount         () const override;
            gfx::ITexture *         GetTexture              (MaterialTextureType _type) const override;
            const core::string &    GetTexturePath          (MaterialTextureType _type) const override;

            void                    SetTexture              (MaterialTextureType _type, gfx::ITexture * _texture) final;

            core::uint              getTextureCount         () const { return (core::uint)core::countof(m_textureInfos); }
            gfx::Texture *          getTexture              (MaterialTextureType _type) const;
            const core::string &    getTexturePath          (MaterialTextureType _type) const { return m_textureInfos[core::asInteger(_type)].path; };

            static MaterialModel *  createFromImporterData  (const MaterialImporterData & _data);

        private:
            Flags                   m_flags;
            struct TextureInfo
            {
                core::string        path;
                gfx::Texture *      texture = nullptr;
            };
            TextureInfo m_textureInfos[core::enumCount<MaterialTextureType>()];
        };
    }
}