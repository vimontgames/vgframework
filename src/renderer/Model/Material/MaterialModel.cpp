#include "renderer/Precomp.h"
#include "MaterialModel.h"
#include "core/Object/AutoRegisterClass.h"
#include "gfx/Resource/Texture.h"
#include "renderer/Importer/SceneImporterData.h"
#include "renderer/Renderer.h"
#include "core/Object/EnumHelper.h"
#include "DefaultMaterial/DefaultMaterialModel.hpp"

using namespace vg::core;

namespace vg::renderer
{
    VG_AUTO_REGISTER_CLASS(MaterialModel);

    //--------------------------------------------------------------------------------------
    bool MaterialModel::registerClass(core::IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassHelper(MaterialModel, "Material Model", IClassDesc::Flags::Model))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MaterialModel::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerPropertyEnumBitfield(MaterialModel, Flags, m_flags, "Flags");

        return true;
    }

    //--------------------------------------------------------------------------------------
    MaterialModel::MaterialModel(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent),
        m_flags((Flags)0x0)
    {

    }

    //--------------------------------------------------------------------------------------
    MaterialModel::~MaterialModel()
    {
        for (uint t = 0; t < core::enumCount<MaterialTextureType>(); ++t)
            VG_SAFE_RELEASE(m_textureInfos[t].texture);
    }

    //--------------------------------------------------------------------------------------
    MaterialModel * MaterialModel::createFromImporterData(const MaterialImporterData & _data)
    {
        MaterialModel * matModel = new MaterialModel(_data.name);

        for (uint t = 0; t < core::enumCount<MaterialTextureType>(); ++t)
            matModel->m_textureInfos[t].path = _data.texturePath[t];

        return matModel;
    }

    //--------------------------------------------------------------------------------------
    gfx::ITexture * MaterialModel::GetTexture(MaterialTextureType _type) const
    {
        VG_ASSERT(asInteger(_type) < countof(m_textureInfos));
        return getTexture(_type);
    }

    //--------------------------------------------------------------------------------------
    gfx::Texture * MaterialModel::getTexture(MaterialTextureType _type) const
    { 
        auto * tex = m_textureInfos[asInteger(_type)].texture;
        if (tex)
            return tex;
        else
            return Renderer::get()->getDefaultTexture(_type);
    }

    //--------------------------------------------------------------------------------------
    core::uint MaterialModel::GetTextureCount() const
    {
        return getTextureCount();
    }

    //--------------------------------------------------------------------------------------
    const core::string & MaterialModel::GetTexturePath(MaterialTextureType _type) const
    {
        VG_ASSERT(asInteger(_type) < countof(m_textureInfos));
        return getTexturePath(_type);
    }

    //--------------------------------------------------------------------------------------
    void MaterialModel::SetTexture(MaterialTextureType _type, gfx::ITexture * _texture)
    {
        auto & tex = m_textureInfos[asInteger(_type)].texture;

        if (tex != _texture)
        {
            VG_SAFE_INCREASE_REFCOUNT(_texture);
            VG_SAFE_RELEASE(tex);
            tex = (gfx::Texture*)_texture;
        }
    }
}