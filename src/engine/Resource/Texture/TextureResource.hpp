#include "TextureResource.h"
#include "gfx/ITexture.h"
#include "core/File/File.h"

#include "TextureResourceMeta.hpp"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_RESOURCE_CLASS(TextureResource, "Texture", ResourcePriority::Texture);

    //--------------------------------------------------------------------------------------
    bool TextureResource::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

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
                       ext.push_back(".jpeg");
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
        return Engine::get()->GetRenderer()->CookTexture(_file, &meta->getTextureImportSettings());
    }

    //--------------------------------------------------------------------------------------
    LoadStatus TextureResource::Load(const core::string & _file, core::IObject *& _object)
    {
        VG_ASSERT(Kernel::getScheduler()->IsLoadingThread());
        return Engine::get()->GetRenderer()->LoadTexture(_file, (gfx::ITexture*&)_object);
    }

    //--------------------------------------------------------------------------------------
    core::IResourceMeta * TextureResource::CreateResourceMeta(const core::string & _path) const
    {
        return new TextureResourceMeta(_path);
    }   
}