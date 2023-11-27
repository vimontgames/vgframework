#include "TextureResource.h"
#include "gfx/ITexture.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_RESOURCE_CLASS(TextureResource, "Texture");

    //--------------------------------------------------------------------------------------
    bool TextureResource::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        // Display texture
        //_desc.registerProperty("Resource", "m_object", (IObject **)offsetof(TextureResource, m_object), "Object", IProperty::Flags::NotSaved);

        return true;
    }

    //--------------------------------------------------------------------------------------
    TextureResource::TextureResource(const core::string & _name, core::IObject * _parent) :
        Resource(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    TextureResource::~TextureResource()
    {
        ResourceManager::get()->unloadResource(this, GetResourcePath());
    }

    //--------------------------------------------------------------------------------------
    const core::vector<core::string> TextureResource::getExtensions() const
    {
        vector<string> ext;
                       ext.push_back(".psd");
                       ext.push_back(".tga");
                       ext.push_back(".jpg");
                       ext.push_back(".png");
        return ext;
    }

    //--------------------------------------------------------------------------------------
    void TextureResource::onResourcePathChanged(const string & _oldPath, const string & _newPath)
    {
        if (_oldPath != _newPath)
            ResourceManager::get()->loadResourceAsync(this, _oldPath, _newPath);
    }

    //--------------------------------------------------------------------------------------
    bool TextureResource::cook(const string & _file) const
    {
        return Engine::get()->GetRenderer()->cookTexture(_file);
    }

    //--------------------------------------------------------------------------------------
    core::IObject * TextureResource::load(const string & _file)
    {
        gfx::ITexture * texture = Engine::get()->GetRenderer()->loadTexture(_file);
        return texture;
    }
}