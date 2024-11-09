#include "TextureResource.h"
#include "gfx/ITexture.h"
#include "core/File/File.h"

#include "TextureResourceMeta.hpp"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_RESOURCE_CLASS(TextureResource, "Texture");

    //--------------------------------------------------------------------------------------
    bool TextureResource::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        //auto * rm = ResourceManager::get();
        //
        //rm->RegisterExtension("TextureResource", ".psd");
        //rm->RegisterExtension("TextureResource", ".tga");
        //rm->RegisterExtension("TextureResource", ".jpg");
        //rm->RegisterExtension("TextureResource", ".png");

        return true;
    }

    //--------------------------------------------------------------------------------------
    TextureResource::TextureResource(const string & _name, IObject * _parent) :
        Resource(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    TextureResource::~TextureResource()
    {
        
    }

    //--------------------------------------------------------------------------------------
    const vector<string> TextureResource::GetExtensions() const
    {
        vector<string> ext;
                       ext.push_back(".hdr");
                       ext.push_back(".jpg");
                       ext.push_back(".png");
                       ext.push_back(".psd");
                       ext.push_back(".tga");                       
        return ext;
    }

    //--------------------------------------------------------------------------------------
    bool TextureResource::Cook(const string & _file) const
    {
        // Get meta if it exists
        auto * meta = (TextureResourceMeta*)ResourceManager::get()->GetOrCreateResourceMeta(this);

        // Import texture using meta
        return Engine::get()->GetRenderer()->cookTexture(_file, &meta->getTextureImportSettings());
    }

    //--------------------------------------------------------------------------------------
    IObject * TextureResource::Load(const string & _file)
    {
        gfx::ITexture * texture = Engine::get()->GetRenderer()->loadTexture(_file);
        return texture;
    }

    //--------------------------------------------------------------------------------------
    core::IResourceMeta * TextureResource::CreateResourceMeta(const core::string & _path) const
    {
        return new TextureResourceMeta(_path);
    }   
}