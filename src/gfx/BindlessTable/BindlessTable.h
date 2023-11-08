#pragma once

#include "core/Object/Object.h"
#include "core/Pool/IndexPool.h"
#include "BindlessTable_consts.h"
#include "gfx/RootSignature/RootSignatureTableDesc.h"

namespace vg::gfx
{
    class Texture;
    class Buffer;

    namespace base
    {
        class BindlessTable : public core::Object
        {
        public:
            BindlessTable();
            ~BindlessTable();

            const RootSignatureTableDesc & getTableDesc() const { return m_tableDesc; }

            BindlessTextureHandle allocBindlessTextureHandle(const gfx::Texture * _texture, ReservedSlot _reservedSlot = ReservedSlot::None);
            void freeBindlessTextureHandle(BindlessTextureHandle & _handle);

            BindlessBufferHandle allocBindlessBufferHandle(const gfx::Buffer * _buffer, ReservedSlot _reservedSlot = ReservedSlot::None);
            void freeBindlessBufferHandle(BindlessBufferHandle & _handle);

            BindlessRWTextureHandle allocBindlessRWTextureHandle(const gfx::Texture * _texture, ReservedSlot _reservedSlot = ReservedSlot::None);
            void freeBindlessRWTextureHandle(BindlessRWTextureHandle & _handle);

            BindlessRWBufferHandle allocBindlessRWBufferHandle(const gfx::Buffer * _buffer, ReservedSlot _reservedSlot = ReservedSlot::None);
            void freeBindlessRWBufferHandle(BindlessRWBufferHandle & _handle);
            
        private:
            template <class H, class T, class P> H allocBindlessHandle(const T * _resource, ReservedSlot _reservedSlot, P & _pool, T ** _resources, core::uint _offset, core::uint _invalid);
            template <class H, class T, class P> void freeBindlessHandle(H & _handle, P & _pool, T ** _resources, core::uint _offset, core::uint _invalid);

        private:
            RootSignatureTableDesc                                          m_tableDesc;

            core::IndexPool<BindlessHandle::Type, BINDLESS_TEXTURE_COUNT>   m_textureIndexPool;
            Texture *                                                       m_textures[BINDLESS_TEXTURE_COUNT];

            core::IndexPool<BindlessHandle::Type, BINDLESS_BUFFER_COUNT>    m_bufferIndexPool;
            Buffer *                                                        m_buffers[BINDLESS_BUFFER_COUNT];

            core::IndexPool<BindlessHandle::Type, BINDLESS_RWTEXTURE_COUNT> m_rwTextureIndexPool;
            Texture *                                                       m_rwTextures[BINDLESS_RWTEXTURE_COUNT];

            core::IndexPool<BindlessHandle::Type, BINDLESS_RWBUFFER_COUNT>  m_rwBufferIndexPool;
            Buffer *                                                        m_rwBuffers[BINDLESS_RWBUFFER_COUNT];
        };
    }
}

#include VG_GFXAPI_HEADER(BindlessTable)

namespace vg::gfx
{
    class BindlessTable : public VG_GFXAPI::BindlessTable
    {
        using super = VG_GFXAPI::BindlessTable;

    public:
        const char * getClassName() const final { return "BindlessTable"; }

        BindlessTable();
        ~BindlessTable();

        void init();

    private:
        gfx::Texture *   m_defaultTexture = nullptr;
    };
}