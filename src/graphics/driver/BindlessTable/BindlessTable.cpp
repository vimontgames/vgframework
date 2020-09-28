#include "graphics/driver/Precomp.h"
#include "BindlessTable.h"
#include "graphics/driver/device/Device.h"
#include "graphics/driver/Resource/Texture.h"

using namespace vg::core;

#include VG_GFXAPI_IMPL(BindlessTable)

namespace vg::graphics::driver
{
    namespace base
    {
        //--------------------------------------------------------------------------------------
        BindlessTable::BindlessTable()
        {
            m_tableDesc.setShaderStageFlags(ShaderStageFlags::All);
            m_tableDesc.addTextures(0, max_bindless_elements, 0, 0);
        }

        //--------------------------------------------------------------------------------------
        BindlessTable::~BindlessTable()
        {
            
        }

        //--------------------------------------------------------------------------------------
        // In case the value of '_reservedSlot' is not ReservedSlot::None then its value casted to 'BindlessTextureHandle' 
        // will be used (slot must be free)
        //--------------------------------------------------------------------------------------
        BindlessTextureHandle BindlessTable::allocBindlessTextureHandle(driver::Texture * _texture, ReservedSlot _reservedSlot)
        {
            BindlessTextureHandle handle;
            
            if (ReservedSlot::None == _reservedSlot)
                handle = m_srvIndexPool.alloc();
            else
            {
                VG_ASSERT(m_srvIndexPool.isAvailable((BindlessHandle)_reservedSlot));
                handle = (BindlessHandle)_reservedSlot;
            }

            if (invalidBindlessTextureSRVHandle != handle && ReservedSlot::None != _reservedSlot)
            {
                auto & desc = m_slotDesc[handle];
                desc.flags = Flags::Texture;
                desc.texture = _texture;
            }

            return handle;
        }

        //--------------------------------------------------------------------------------------
        void BindlessTable::freeBindlessTextureHandle(BindlessTextureHandle & _handle)
        {
            if (invalidBindlessTextureSRVHandle != _handle)
            {
                m_srvIndexPool.free(_handle);
                _handle = invalidBindlessTextureSRVHandle;
                auto & desc = m_slotDesc[_handle];
                desc.flags = (Flags)0;
                desc.texture = nullptr;
            }            
        }
    }

    //--------------------------------------------------------------------------------------
    BindlessTable::BindlessTable() :
        super::BindlessTable()
    {
 
    }

    //--------------------------------------------------------------------------------------
    BindlessTable::~BindlessTable()
    {
        VG_SAFE_RELEASE(m_defaultTexture);
    }

    //--------------------------------------------------------------------------------------
    void BindlessTable::init()
    {
        auto * device = driver::Device::get();
        
        const uint w = 32;
        const uint h = 32;
        TextureDesc texDesc = TextureDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, TextureType::Texture2D, PixelFormat::R8G8B8A8_unorm, TextureFlags::None, w, h);
        u32 texInitData[h][w];
        for (u32 j = 0; j < h; ++j)
            for (u32 i = 0; i < w; ++i)
                texInitData[j][i] = ((i>>3) & 1) != ((j>>3) & 1) ? 0xFFFF00FF : 0x7F7F007F;
        
        // create default texture at slot 'invalidBindlessTextureHandle'
        m_defaultTexture = device->createTexture(texDesc, "testTex", (void*)texInitData, ReservedSlot(invalidBindlessTextureSRVHandle));
        VG_ASSERT(m_defaultTexture->getBindlessTextureHandle() == invalidBindlessTextureSRVHandle);

        // copy texture to all 'texture' slots
        for (uint i = 0; i < max_bindless_elements; ++i)
            if (invalidBindlessTextureSRVHandle != i)
                copyTextureHandle(i, m_defaultTexture);
    }
}