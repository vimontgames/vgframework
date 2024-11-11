#include "gfx/Precomp.h"
#include "BindlessTable.h"
#include "gfx/device/Device.h"
#include "gfx/Raytracing/TLAS.h"
#include "gfx/Resource/Texture.h"
#include "gfx/Resource/Buffer.h"
#include "core/Math/Math.h"

using namespace vg::core;

#include VG_GFXAPI_IMPL(BindlessTable)

namespace vg::gfx
{
    namespace base
    {
        //--------------------------------------------------------------------------------------
        // TODO : Use DynamicResources to clean up this mess with aliasing different texture type descriptors?
        // https://microsoft.github.io/DirectX-Specs/d3d/HLSL_SM_6_6_DynamicResources.html
        //--------------------------------------------------------------------------------------
        BindlessTable::BindlessTable()
        {
            m_tableDesc.setShaderStageFlags(ShaderStageFlags::All);
            
            #ifdef VG_VULKAN
            m_tableDesc.addTextures(BINDLESS_TEXTURE_BINDING,               BINDLESS_TEXTURE_START,     BINDLESS_TEXTURE_COUNT);
            #else
            m_tableDesc.addTextures(BINDLESS_TEXTURE_BINDING_1D,            BINDLESS_TEXTURE_START,     BINDLESS_TEXTURE_COUNT);
            m_tableDesc.addTextures(BINDLESS_TEXTURE_BINDING_2D,            BINDLESS_TEXTURE_START,     BINDLESS_TEXTURE_COUNT);
            m_tableDesc.addTextures(BINDLESS_TEXTURE_BINDING_CUBE,          BINDLESS_TEXTURE_START,     BINDLESS_TEXTURE_COUNT);
            m_tableDesc.addTextures(BINDLESS_TEXTURE_BINDING_2DMS,          BINDLESS_TEXTURE_START,     BINDLESS_TEXTURE_COUNT);
            m_tableDesc.addTextures(BINDLESS_TEXTURE_BINDING_2D_UINT2,      BINDLESS_TEXTURE_START,     BINDLESS_TEXTURE_COUNT);
            m_tableDesc.addTextures(BINDLESS_TEXTURE_BINDING_3D,            BINDLESS_TEXTURE_START,     BINDLESS_TEXTURE_COUNT);
            #endif

            m_tableDesc.addBuffers(BINDLESS_BUFFER_BINDING,                 BINDLESS_BUFFER_START,      BINDLESS_BUFFER_COUNT);

            #ifdef VG_VULKAN
            m_tableDesc.addRWTextures(BINDLESS_RWTEXTURE_BINDING,           BINDLESS_RWTEXTURE_START,   BINDLESS_RWTEXTURE_COUNT);
            #else
            m_tableDesc.addRWTextures(BINDLESS_RWTEXTURE_BINDING_1D,        BINDLESS_RWTEXTURE_START,   BINDLESS_RWTEXTURE_COUNT);
            m_tableDesc.addRWTextures(BINDLESS_RWTEXTURE_BINDING_2D,        BINDLESS_RWTEXTURE_START,   BINDLESS_RWTEXTURE_COUNT);
            m_tableDesc.addRWTextures(BINDLESS_RWTEXTURE_BINDING_2D_FLOAT2, BINDLESS_RWTEXTURE_START,   BINDLESS_RWTEXTURE_COUNT);
            m_tableDesc.addRWTextures(BINDLESS_RWTEXTURE_BINDING_2D_ARRAY,  BINDLESS_RWTEXTURE_START,   BINDLESS_RWTEXTURE_COUNT);
            m_tableDesc.addRWTextures(BINDLESS_RWTEXTURE_BINDING_3D,        BINDLESS_RWTEXTURE_START,   BINDLESS_RWTEXTURE_COUNT);
            #endif

            m_tableDesc.addRWBuffers(BINDLESS_RWBUFFER_BINDING,             BINDLESS_RWBUFFER_START,    BINDLESS_RWBUFFER_COUNT);
            m_tableDesc.addTLAS(BINDLESS_TLAS_BINDING,                      BINDLESS_TLAS_START,        BINDLESS_TLAS_COUNT);
        }

        //--------------------------------------------------------------------------------------
        BindlessTable::~BindlessTable()
        {
            
        }

        //--------------------------------------------------------------------------------------
        template <class H, class T, class P> H BindlessTable::allocBindlessHandle(const T * _resource, ReservedSlot _reservedSlot, P & _pool, T ** _resources, uint _offset, uint _invalid)
        {
            H handle = H(_invalid);

            if (ReservedSlot::None == _reservedSlot)
            {
                handle = H(_pool.alloc() + _offset);
            }
            else
            {
                u16 reservedSlotIndex = (u16)_reservedSlot- _offset;
                VG_ASSERT(_pool.isAvailable(reservedSlotIndex), "[BindlessTable] ReservedSlot %u is already used", (u16)_reservedSlot);
                if (_pool.isAvailable(reservedSlotIndex))
                {
                    handle = H((u16)_reservedSlot);
                    _resources[handle - _offset] = (T *)_resource;
                }
            }

            return handle;
        }

        //--------------------------------------------------------------------------------------
        template <class H, class T, class P> void BindlessTable::freeBindlessHandle(H & _handle, P & _pool, T ** _resources, uint _offset, uint _invalid)
        {
            if (_invalid != _handle)
            {
                _pool.dealloc(_handle);
                _resources[_handle - _offset] = nullptr;
                _handle = H(_invalid);
            }
        }

        //--------------------------------------------------------------------------------------
        BindlessTextureHandle BindlessTable::allocBindlessTextureHandle(const gfx::Texture * _texture, ReservedSlot _reservedSlot)
        {
            VG_ASSERT(ReservedSlot::None == _reservedSlot || BindlessTextureHandle((u16)_reservedSlot).isValid(), "Reserved slot %u does not belong to the Texture SRV range [%u;%u]", _reservedSlot, (uint)BindlessTextureHandle::getValidRange().x, (uint)BindlessTextureHandle::getValidRange().y);
            return allocBindlessHandle<BindlessTextureHandle>(_texture, _reservedSlot, m_textureIndexPool, m_textures, BINDLESS_TEXTURE_START, BINDLESS_TEXTURE_INVALID);
        }

        //--------------------------------------------------------------------------------------
        void BindlessTable::freeBindlessTextureHandle(BindlessTextureHandle & _handle)
        {
            VG_ASSERT(_handle.isValid(), "0x%04X is not a valid Texture handle", (u16)_handle);
            freeBindlessHandle(_handle, m_textureIndexPool, m_textures, BINDLESS_TEXTURE_START, BINDLESS_TEXTURE_INVALID);
        }
        
        //--------------------------------------------------------------------------------------
        BindlessBufferHandle BindlessTable::allocBindlessBufferHandle(const gfx::Buffer * _buffer, ReservedSlot _reservedSlot)
        {
            VG_ASSERT(ReservedSlot::None == _reservedSlot || BindlessBufferHandle((u16)_reservedSlot).isValid(), "Reserved slot %u does not belong to the Buffer SRV range [%u;%u]", _reservedSlot, (uint)BindlessBufferHandle::getValidRange().x, (uint)BindlessBufferHandle::getValidRange().y);
            return allocBindlessHandle<BindlessBufferHandle>(_buffer, _reservedSlot, m_bufferIndexPool, m_buffers, BINDLESS_BUFFER_START, BINDLESS_BUFFER_INVALID);
        }
        
        //--------------------------------------------------------------------------------------
        void BindlessTable::freeBindlessBufferHandle(BindlessBufferHandle & _handle)
        {
            VG_ASSERT(_handle.isValid(), "0x%04X is not a valid Buffer handle", (u16)_handle);
            freeBindlessHandle(_handle, m_bufferIndexPool, m_buffers, BINDLESS_BUFFER_START, BINDLESS_BUFFER_INVALID);
        }

        //--------------------------------------------------------------------------------------
        BindlessRWTextureHandle BindlessTable::allocBindlessRWTextureHandle(const gfx::Texture * _texture, ReservedSlot _reservedSlot)
        {
            VG_ASSERT(ReservedSlot::None == _reservedSlot || BindlessRWTextureHandle((u16)_reservedSlot).isValid(), "Reserved slot %u does not belong to the RWTexture range [%u;%u]", _reservedSlot, (uint)BindlessRWTextureHandle::getValidRange().x, (uint)BindlessRWTextureHandle::getValidRange().y);
            return allocBindlessHandle<BindlessRWTextureHandle>(_texture, _reservedSlot, m_rwTextureIndexPool, m_rwTextures, BINDLESS_RWTEXTURE_START, BINDLESS_RWTEXTURE_INVALID);
        }

        //--------------------------------------------------------------------------------------
        void BindlessTable::freeBindlessRWTextureHandle(BindlessRWTextureHandle & _handle)
        {
            VG_ASSERT(_handle.isValid(), "0x%04X is not a valid RWTexture handle", (u16)_handle);
            freeBindlessHandle(_handle, m_rwTextureIndexPool, m_rwTextures, BINDLESS_RWTEXTURE_START, BINDLESS_RWTEXTURE_INVALID);
        }

        //--------------------------------------------------------------------------------------
        BindlessRWBufferHandle BindlessTable::allocBindlessRWBufferHandle(const gfx::Buffer * _buffer, ReservedSlot _reservedSlot)
        {
            VG_ASSERT(ReservedSlot::None == _reservedSlot || BindlessRWBufferHandle((u16)_reservedSlot).isValid(), "Reserved slot %u does not belong to the RWBuffer range [%u;%u]", _reservedSlot, (uint)BindlessRWBufferHandle::getValidRange().x, (uint)BindlessRWBufferHandle::getValidRange().y);
            return allocBindlessHandle<BindlessRWBufferHandle>(_buffer, _reservedSlot, m_rwBufferIndexPool, m_rwBuffers, BINDLESS_RWBUFFER_START, BINDLESS_RWBUFFER_INVALID);
        }

        //--------------------------------------------------------------------------------------
        void BindlessTable::freeBindlessRWBufferHandle(BindlessRWBufferHandle & _handle)
        {
            VG_ASSERT(_handle.isValid(), "0x%04X is not a valid RWBuffer handle", (u16)_handle);
            freeBindlessHandle(_handle, m_rwBufferIndexPool, m_rwBuffers, BINDLESS_RWBUFFER_START, BINDLESS_RWBUFFER_INVALID);
        }

        //--------------------------------------------------------------------------------------
        BindlessTLASHandle BindlessTable::allocBindlessTLASHandle(const gfx::Buffer * _buffer, ReservedSlot _reservedSlot)
        {
            VG_ASSERT(ReservedSlot::None == _reservedSlot || BindlessTLASHandle((u16)_reservedSlot).isValid(), "Reserved slot %u does not belong to the TLAS range [%u;%u]", _reservedSlot, (uint)BindlessTLASHandle::getValidRange().x, (uint)BindlessTLASHandle::getValidRange().y);
            return allocBindlessHandle<BindlessTLASHandle>(_buffer, _reservedSlot, m_TLASIndexPool, m_TLAS, BINDLESS_TLAS_START, BINDLESS_TLAS_INVALID);
        }

        //--------------------------------------------------------------------------------------
        void BindlessTable::freeBindlessTLASHandle(BindlessTLASHandle & _handle)
        {
            VG_ASSERT(_handle.isValid(), "0x%04X is not a valid TLAS handle", (u16)_handle);
            freeBindlessHandle(_handle, m_TLASIndexPool, m_TLAS, BINDLESS_TLAS_START, BINDLESS_TLAS_INVALID);
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
        VG_SAFE_RELEASE(m_defaultTexture2D);
        VG_SAFE_RELEASE(m_defaultTextureCube);
    }

    //--------------------------------------------------------------------------------------
    void BindlessTable::init()
    {
        auto * device = gfx::Device::get();
        
        // create default texture 2D
        {
            const uint w = 16;
            const uint h = 16;
            TextureDesc texDesc = TextureDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, TextureType::Texture2D, PixelFormat::R8G8B8A8_unorm, TextureFlags::None, w, h);
            u32 texInitData[h][w];
            for (uint j = 0; j < h; ++j)
                for (uint i = 0; i < w; ++i)
                    texInitData[j][i] = ((i >> 3) & 1) != ((j >> 3) & 1) ? 0xFFFF00FF : 0x7F7F007F;

            m_defaultTexture2D = device->createTexture(texDesc, "Default_Texture2D", (void *)texInitData, ReservedSlot::InvalidTexture2D);
            VG_ASSERT(m_defaultTexture2D->getTextureHandle() == BINDLESS_TEXTURE_INVALID);
        }
        
        // copy texture to all 'texture' slots
        for (uint i = BINDLESS_TEXTURE_START; i < BINDLESS_TEXTURE_COUNT; ++i)
            if (BINDLESS_TEXTURE_INVALID != i)
                copyTextureHandle(i, m_defaultTexture2D);

        // create default cubemap
        {
            const uint w = 16;
            const uint h = 16;
            TextureDesc texDesc = TextureDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, TextureType::TextureCube, PixelFormat::R8G8B8A8_unorm, TextureFlags::None, w, h, 6);

            u32 texInitData[6][h][w];
            for (uint f = 0; f < 6; ++f)
            {
                u32 color[2];
                switch ((CubemapFace)f)
                {
                    case CubemapFace::PositiveX:
                        color[0] = 0xFF0000FF;
                        color[1] = 0xFF00007F;
                        break;

                    case CubemapFace::NegativeX:
                        color[0] = 0xFF00007F;
                        color[1] = 0xFF00003F;
                        break;

                    case CubemapFace::PositiveY:
                        color[0] = 0xFF00FF00;
                        color[1] = 0xFF007F00;
                        break;

                    case CubemapFace::NegativeY:
                        color[0] = 0xFF007F00;
                        color[1] = 0xFF003F00;
                        break;

                    case CubemapFace::PositiveZ:
                        color[0] = 0xFFFF0000;
                        color[1] = 0xFF7F0000;
                        break;

                    case CubemapFace::NegativeZ:
                        color[0] = 0xFF7F0000;
                        color[1] = 0xFF3F0000;
                        break;
                }

                for (u32 j = 0; j < h; ++j)
                    for (u32 i = 0; i < w; ++i)
                        texInitData[f][j][i] = ((i >> 2) & 1) != ((j >> 2) & 1) ? color[0] : color[1];
            }

            m_defaultTextureCube = device->createTexture(texDesc, "Default_Cube", (void *)texInitData, ReservedSlot::InvalidTextureCube);
            VG_ASSERT(m_defaultTextureCube->getTextureHandle() == BINDLESS_TEXTURE_INVALID - 1);
        }

        // TODO: initialize other buffer types?
    }

    //--------------------------------------------------------------------------------------
    void BindlessTable::updateBindlessTLASHandle(const BindlessTLASHandle & _handle, const gfx::TLAS * _tlas)
    {
        super::updateBindlessTLASHandle(_handle, _tlas);
    }
}