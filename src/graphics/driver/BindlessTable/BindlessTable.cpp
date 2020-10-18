#include "graphics/driver/Precomp.h"
#include "BindlessTable.h"
#include "graphics/driver/device/Device.h"
#include "graphics/driver/Resource/Texture.h"
#include "core/Math/Math.h"

#include "Shaders/system/bindless.hlsli"

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

            m_tableDesc.addTextures(0, bindless_texture_SRV_count, 0, bindless_texture_SRV_offset);
            m_tableDesc.addBuffers(0, bindless_buffer_SRV_count, 1, bindless_buffer_SRV_offset);
        }

        //--------------------------------------------------------------------------------------
        BindlessTable::~BindlessTable()
        {
            
        }

        //--------------------------------------------------------------------------------------
        template <class H, class T, class P> H BindlessTable::allocBindlessHandle(T * _resource, ReservedSlot _reservedSlot, P & _pool, T ** _resources, uint _offset, uint _invalid)
        {
            H handle;

            if (ReservedSlot::None == _reservedSlot)
            {
                handle = _pool.alloc() + _offset;
            }
            else
            {
                VG_ASSERT(_pool.isAvailable(_reservedSlot));
                handle = _reservedSlot;
            }

            if (_invalid != handle && ReservedSlot::None != _reservedSlot)
                _resources[handle - _offset] = _resource;

            return handle;
        }

        //--------------------------------------------------------------------------------------
        template <class H, class T, class P> void BindlessTable::freeBindlessHandle(H & _handle, P & _pool, T ** _resources, uint _offset, uint _invalid)
        {
            if (_invalid != _handle)
            {
                _pool.free(_handle);
                _resources[_handle - _offset] = nullptr;
                _handle = _invalid;
            }
        }

        //--------------------------------------------------------------------------------------
        BindlessTextureSrvHandle BindlessTable::allocBindlessTextureHandle(driver::Texture * _texture, ReservedSlot _reservedSlot)
        {
            return allocBindlessHandle<BindlessTextureSrvHandle>(_texture, _reservedSlot, m_textureSrvIndexPool, m_textureSrv, bindless_texture_SRV_offset, bindless_texture_SRV_invalid);
        }

        //--------------------------------------------------------------------------------------
        void BindlessTable::freeBindlessTextureHandle(BindlessTextureSrvHandle & _handle)
        {
            freeBindlessHandle(_handle, m_textureSrvIndexPool, m_textureSrv, bindless_texture_SRV_offset, bindless_texture_SRV_invalid);
        }
        
        //--------------------------------------------------------------------------------------
        BindlessBufferSrvHandle BindlessTable::allocBindlessBufferHandle(driver::Buffer * _buffer, ReservedSlot _reservedSlot)
        {
            return allocBindlessHandle<BindlessBufferSrvHandle>(_buffer, _reservedSlot, m_bufferSrvIndexPool, m_bufferSrv, bindless_buffer_SRV_offset, bindless_buffer_SRV_invalid);
        }
        
        //--------------------------------------------------------------------------------------
        void BindlessTable::freeBindlessBufferHandle(BindlessBufferSrvHandle & _handle)
        {
            freeBindlessHandle(_handle, m_bufferSrvIndexPool, m_bufferSrv, bindless_buffer_SRV_offset, bindless_buffer_SRV_invalid);
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
        m_defaultTexture = device->createTexture(texDesc, "testTex", (void*)texInitData, ReservedSlot(bindless_texture_SRV_invalid));
        VG_ASSERT(m_defaultTexture->getBindlessSRVHandle() == bindless_texture_SRV_invalid);
        
        // copy texture to all 'texture' slots
        for (uint i = bindless_texture_SRV_offset; i < bindless_texture_SRV_count; ++i)
            if (bindless_texture_SRV_invalid != i)
                copyTextureHandle(i, m_defaultTexture);
    }
}