#include "MaterialResource.h"
#include "core/File/File.h"
#include "graphics/renderer/IMeshModel.h"
#include "graphics/renderer/IMaterialModel.h"

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

    }

    //--------------------------------------------------------------------------------------
    const core::vector<core::string> MaterialResource::getExtensions() const
    {
        vector<string> ext;
        ext.push_back(".mat");
        return ext;
    }

    //--------------------------------------------------------------------------------------
    void MaterialResource::onPathChanged(IObject * _owner, const string & _oldPath, const string & _newPath)
    {
      
    }

    //--------------------------------------------------------------------------------------
    bool MaterialResource::cook(const string & _file)
    {
        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MaterialResource::load(const string & _path, IObject * _owner)
    {
        return true;
    }

    //--------------------------------------------------------------------------------------
    void MaterialResource::onResourceLoaded(core::IResource * _resource)
    {
        
    }

    //--------------------------------------------------------------------------------------
    uint MaterialResource::getSubResourceCount() const
    {
        return m_textureResources.count();
    }

    //--------------------------------------------------------------------------------------
    IResource * MaterialResource::getSubResource(uint _index)
    {
        return &m_textureResources[_index];
    }
}