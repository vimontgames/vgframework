#include "TextureResource.h"
#include "graphics/driver/ITexture.h"

using namespace vg::core;

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    TextureResource::TextureResource(IObject * _owner) :
        Resource(_owner)
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
    void TextureResource::onPathChanged(IObject * _owner, const string & _oldPath, const string & _newPath)
    {
        if (_oldPath != _newPath)
            ResourceManager::get()->loadResourceAsync(this, _newPath, _owner);
    }

    //--------------------------------------------------------------------------------------
    bool TextureResource::cook(const string & _file)
    {
        return Engine::get()->getRenderer()->cookTexture(_file);
    }

    //--------------------------------------------------------------------------------------
    bool TextureResource::load(const string & _file, IObject * _owner)
    {
        VG_SAFE_RELEASE(m_object);

        m_object = Engine::get()->getRenderer()->loadTexture(_file);

        return nullptr != m_object;
    }
}