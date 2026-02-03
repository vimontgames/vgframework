#include "TextureResource.h"
#include "gfx/ITexture.h"
#include "core/File/File.h"
#include "Shaders/system/table.hlsli"
#include "TextureResourceMeta.hpp"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_RESOURCE_CLASS(TextureResource, "Texture", ResourcePriority::Texture);

    declareResizeVectorDefaultFunc(TextureResource);

    //--------------------------------------------------------------------------------------
    bool TextureResource::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerResizeVectorDefaultFunc(TextureResource);

        return true;
    }

    //--------------------------------------------------------------------------------------
    TextureResource::TextureResource(const string & _name, IObject * _parent) :
        Resource(_name, _parent)
    {
        SetUserData((ResourceUserData)ReservedSlot::None);
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
    ReservedSlot TextureResource::getReservedSlot() const
    {
        const ResourceUserData data = GetUserData();
        VG_ASSERT(0 != data);
        return (ReservedSlot)data;
    }

    //--------------------------------------------------------------------------------------
    LoadStatus TextureResource::Load(const core::string & _file, core::IObject *& _object)
    {
        VG_ASSERT(Kernel::getScheduler()->IsLoadingThread());
        return Engine::get()->GetRenderer()->LoadTexture(_file, (gfx::ITexture*&)_object, getReservedSlot());
    }

    //--------------------------------------------------------------------------------------
    core::IResourceMeta * TextureResource::CreateResourceMeta(const core::string & _path) const
    {
        return new TextureResourceMeta(_path);
    }   
}