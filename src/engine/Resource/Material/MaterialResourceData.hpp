#include "MaterialResourceData.h"

using namespace vg::core;

namespace vg::engine
{
    VG_AUTO_REGISTER_CLASS(MaterialResourceData);

    //--------------------------------------------------------------------------------------
    bool MaterialResourceData::registerClass(IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassHelper(MaterialResourceData, "Material Resource Data", IClassDesc::Flags::None))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MaterialResourceData::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerPropertyEnumBitfield(MaterialResourceData, renderer::MaterialFlags, m_flags, "Flags");
        _desc.registerPropertyEnumArray(MaterialResourceData, core::float4, renderer::MaterialColorType, m_colors, "Colors", IProperty::Flags::Color);
        _desc.registerPropertyEnumArray(MaterialResourceData, TextureResource, renderer::MaterialTextureType, m_textures, "Textures", IProperty::Flags::Resource);

        return true;
    }

    //--------------------------------------------------------------------------------------
    MaterialResourceData::MaterialResourceData(const core::string & _name, IObject * _parent) :
        Object(_name, _parent)
    {
        m_colors[asInteger(renderer::MaterialColorType::BaseColor)] = float4(1, 1, 1, 1);
        m_colors[asInteger(renderer::MaterialColorType::EmissiveColor)] = float4(0, 0, 0, 0);

        for (auto & tex : m_textures)
            tex.setParent(this);
    }

    //--------------------------------------------------------------------------------------
    MaterialResourceData::~MaterialResourceData()
    {
        
    }

    //--------------------------------------------------------------------------------------
    void MaterialResourceData::onResourceLoaded(core::IResource * _resource)
    {
        IObject * owner = getParent();
        owner->onResourceLoaded(_resource);
    }

    //--------------------------------------------------------------------------------------
    void MaterialResourceData::onResourceUnloaded(IResource * _resource)
    {
        IObject * owner = getParent();
        owner->onResourceUnloaded(_resource);
    }
}