#include "TextureResource.h"
#include "graphics/driver/ITexture.h"

using namespace vg::core;

namespace vg::engine
{
    VG_AUTO_REGISTER_CLASS(TextureResource);

    //--------------------------------------------------------------------------------------
    bool TextureResource::registerClass(IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassHelper(TextureResource, "Texture", IClassDesc::Flags::Resource))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool TextureResource::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

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
    void TextureResource::onResourcePathChanged(IObject * _owner, const string & _oldPath, const string & _newPath)
    {
        if (_oldPath != _newPath)
            ResourceManager::get()->loadResourceAsync(this, _newPath, _owner);
    }

    //--------------------------------------------------------------------------------------
    bool TextureResource::cook(const string & _file)
    {
        return Engine::get()->GetRenderer()->cookTexture(_file);
    }

    //--------------------------------------------------------------------------------------
    bool TextureResource::load(const string & _file, IObject * _owner)
    {
        VG_SAFE_RELEASE(m_object);

        m_object = Engine::get()->GetRenderer()->loadTexture(_file);

        return nullptr != m_object;
    }
}