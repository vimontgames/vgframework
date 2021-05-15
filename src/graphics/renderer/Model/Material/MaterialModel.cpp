#include "graphics/renderer/Precomp.h"
#include "MaterialModel.h"
#include "core/Object/AutoRegisterClass.h"
#include "graphics/driver/Resource/Texture.h"
#include "graphics/renderer/Importer/SceneImporterData.h"

#include "DefaultMaterial/DefaultMaterialModel.hpp"

using namespace vg::core;

namespace vg::graphics::renderer
{
    VG_AUTO_REGISTER_CLASS(MaterialModel);

    //--------------------------------------------------------------------------------------
    bool MaterialModel::registerClass(core::IObjectFactory & _factory)
    {
        if (core::IObjectDescriptor * desc = _factory.registerClassHelper(MaterialModel, "Material Model", IObjectDescriptor::Flags::Model))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MaterialModel::registerProperties(core::IObjectDescriptor & _desc)
    {
        super::registerProperties(_desc);

        //_desc.registerProperty("m_meshGeometry", (core::IObject**)offsetof(MeshModel, m_meshGeometry), "Geometry", IPropertyDescriptor::Flags::None);

        return true;
    }

    //--------------------------------------------------------------------------------------
    MaterialModel::MaterialModel(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    MaterialModel::~MaterialModel()
    {
        for (uint t = 0; t < core::enumCount<MaterialTexture>(); ++t)
            VG_SAFE_RELEASE(m_textureInfos[t].texture);
    }

    //--------------------------------------------------------------------------------------
    MaterialModel * MaterialModel::createFromImporterData(const MaterialImporterData & _data)
    {
        MaterialModel * matModel = new MaterialModel(_data.name);

        matModel->m_textureInfos[asInteger(MaterialTexture::Albedo)].path = _data.texturePath[asInteger(MaterialImporterTexture::Albedo)];
        matModel->m_textureInfos[asInteger(MaterialTexture::Normal)].path = _data.texturePath[asInteger(MaterialImporterTexture::Normal)];

        return matModel;
    }

    //--------------------------------------------------------------------------------------
    driver::ITexture * MaterialModel::GetTexture(core::uint _index) const
    {
        VG_ASSERT(_index < countof(m_textureInfos));
        return getTexture(_index);
    }

    //--------------------------------------------------------------------------------------
    core::uint MaterialModel::GetTextureCount() const
    {
        return getTextureCount();
    }

    //--------------------------------------------------------------------------------------
    const core::string & MaterialModel::GetTexturePath(core::uint _index) const
    {
        VG_ASSERT(_index < countof(m_textureInfos));
        return getTexturePath(_index);
    }

    //--------------------------------------------------------------------------------------
    void MaterialModel::SetTexture(core::uint _index, driver::ITexture * _texture)
    {
        auto & tex = m_textureInfos[_index].texture;

        if (tex != _texture)
        {
            VG_SAFE_INCREASE_REFCOUNT(_texture);
            VG_SAFE_RELEASE(tex);
            tex = (driver::Texture*)_texture;
        }
    }
}