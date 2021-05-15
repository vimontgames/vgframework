#pragma once

#include "graphics/renderer/IMaterialModel.h"

namespace vg::graphics
{
    namespace driver
    {
        class Texture;
    }

    namespace renderer
    {
        class MaterialImporterData;

        enum class MaterialTexture : core::u8
        {
            Albedo = 0,
            Normal
        };

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
            driver::ITexture *      GetTexture      (core::uint _index) const override;
            const core::string &    GetTexturePath  (core::uint _index) const override;

            void                    SetTexture      (core::uint _index, driver::ITexture * _texture) final;

            core::uint              getTextureCount () const                    { return (core::uint)core::countof(m_textureInfos); }
            driver::Texture *       getTexture      (core::uint _index) const   { return m_textureInfos[_index].texture; }
            const core::string &    getTexturePath  (core::uint _index) const   { return m_textureInfos[_index].path; };

            static MaterialModel * createFromImporterData(const MaterialImporterData & _data);

        private:
            struct TextureInfo
            {
                core::string        path;
                driver::Texture *   texture = nullptr;
            };
            TextureInfo m_textureInfos[core::enumCount<MaterialTexture>()];
        };
    }
}