#pragma once

#include "graphics/renderer/IMaterialModel.h"
#include "graphics/renderer/Model/Material/MaterialTextureType.h"

namespace vg::graphics
{
    namespace driver
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
            const char * getClassName() const final { return "MaterialModel"; }

            static bool registerClass(core::IObjectFactory & _factory);
            static bool registerProperties(core::IObjectDescriptor & _desc);

            MaterialModel(const core::string & _name, core::IObject * _parent = nullptr);
            ~MaterialModel();

            core::uint              GetTextureCount () const override;
            driver::ITexture *      GetTexture      (MaterialTextureType _type) const override;
            const core::string &    GetTexturePath  (MaterialTextureType _type) const override;

            void                    SetTexture      (MaterialTextureType _type, driver::ITexture * _texture) final;

            core::uint              getTextureCount () const                            { return (core::uint)core::countof(m_textureInfos); }
            driver::Texture *       getTexture      (MaterialTextureType _type) const;
            const core::string &    getTexturePath  (MaterialTextureType _type) const   { return m_textureInfos[core::asInteger(_type)].path; };

            static MaterialModel * createFromImporterData(const MaterialImporterData & _data);

        private:
            struct TextureInfo
            {
                core::string        path;
                driver::Texture *   texture = nullptr;
            };
            TextureInfo m_textureInfos[core::enumCount<MaterialTextureType>()];
        };
    }
}