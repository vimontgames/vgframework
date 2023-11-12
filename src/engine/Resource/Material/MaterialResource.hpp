#include "MaterialResource.h"
#include "core/File/File.h"
#include "renderer/IMeshModel.h"
#include "renderer/IMaterialModel.h"

using namespace vg::core;

namespace vg::engine
{
    VG_AUTO_REGISTER_CLASS(MaterialResource);

    //--------------------------------------------------------------------------------------
    bool MaterialResource::registerClass(IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassHelper(MaterialResource, "Material Resource", IClassDesc::Flags::Resource))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MaterialResource::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerPropertyObjectVectorHelper(MaterialResource, m_textureResources, TextureResource, "Textures", IProperty::Flags::Resource);

        return true;
    }

    //--------------------------------------------------------------------------------------
    MaterialResource::MaterialResource(const core::string & _name, IObject * _parent) :
        Resource(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    MaterialResource::~MaterialResource()
    {
        ResourceManager::get()->unloadResource(this, GetResourcePath());
    }

    //--------------------------------------------------------------------------------------
    const core::vector<core::string> MaterialResource::getExtensions() const
    {
        vector<string> ext;
        ext.push_back(".mat");
        return ext;
    }

    //--------------------------------------------------------------------------------------
    void MaterialResource::onResourcePathChanged(const string & _oldPath, const string & _newPath)
    {
        VG_ASSERT_NOT_IMPLEMENTED();
    }

    //--------------------------------------------------------------------------------------
    bool MaterialResource::cook(const string & _file) const
    {
        VG_ASSERT_NOT_IMPLEMENTED();
        return true;
    }

    //--------------------------------------------------------------------------------------
    core::IObject * MaterialResource::load(const string & _path)
    {
        VG_ASSERT_NOT_IMPLEMENTED();
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    void MaterialResource::onResourceLoaded(core::IResource * _resource)
    {
        VG_ASSERT_NOT_IMPLEMENTED();
    }

    //--------------------------------------------------------------------------------------
    //uint MaterialResource::getSubResourceCount() const
    //{
    //    return m_textureResources.count();
    //}
    //
    ////--------------------------------------------------------------------------------------
    //IResource * MaterialResource::getSubResource(uint _index)
    //{
    //    return &m_textureResources[_index];
    //}
}