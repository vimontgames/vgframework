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
        BindlessTextureHandle BindlessTable::allocBindlessTextureHandle(driver::Texture * _texture)
        {
            BindlessTextureHandle handle = m_srvIndexPool.alloc();
            if (invalidBindlessTextureHandle != handle)
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
            if (invalidBindlessTextureHandle != _handle)
            {
                m_srvIndexPool.free(_handle);
                _handle = invalidBindlessTextureHandle;
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

        m_defaultTexture = device->createTexture(texDesc, "testTex", (void*)texInitData);
        VG_ASSERT(m_defaultTexture->getBindlessTextureHandle() == 0);
    }
}