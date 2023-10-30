#pragma once

#include "core/Object/Object.h"
#include "core/Pool/IndexPool.h"
#include "BindlessTable_consts.h"
#include "gfx/RootSignature/RootSignatureTableDesc.h"

#include "shaders/system/bindless.hlsli.h"

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

            BindlessTextureSrvHandle allocBindlessTextureHandle(const gfx::Texture * _texture, ReservedSlot _reservedSlot = ReservedSlot::None);
            void freeBindlessTextureHandle(BindlessTextureSrvHandle & _handle);

            BindlessBufferSrvHandle allocBindlessBufferHandle(const gfx::Buffer * _buffer, ReservedSlot _reservedSlot = ReservedSlot::None);
            void freeBindlessBufferHandle(BindlessBufferSrvHandle & _handle);

            BindlessTextureUAVHandle allocBindlessRWTextureHandle(const gfx::Texture * _texture, ReservedSlot _reservedSlot = ReservedSlot::None);
            void freeBindlessRWTextureHandle(BindlessTextureUAVHandle & _handle);

            BindlessBufferUAVHandle allocBindlessRWBufferHandle(const gfx::Buffer * _buffer, ReservedSlot _reservedSlot = ReservedSlot::None);
            void freeBindlessRWBufferHandle(BindlessBufferUAVHandle & _handle);
            
        private:
            template <class H, class T, class P> H allocBindlessHandle(const T * _resource, ReservedSlot _reservedSlot, P & _pool, T ** _resources, core::uint _offset, core::uint _invalid);
            template <class H, class T, class P> void freeBindlessHandle(H & _handle, P & _pool, T ** _resources, core::uint _offset, core::uint _invalid);

        private:
            RootSignatureTableDesc                                                  m_tableDesc;

            core::IndexPool<BindlessHandle::Type, BINDLESS_TEXTURE_SRV_COUNT>       m_textureSrvIndexPool;
            Texture *                                                               m_textureSrv[BINDLESS_TEXTURE_SRV_COUNT];

            core::IndexPool<BindlessHandle::Type, BINDLESS_BUFFER_SRV_COUNT>        m_bufferSrvIndexPool;
            Buffer *                                                                m_bufferSrv[BINDLESS_BUFFER_SRV_COUNT];

            core::IndexPool<BindlessHandle::Type, BINDLESS_TEXTURE_UAV_COUNT>       m_textureUAVIndexPool;
            Texture *                                                               m_textureUAV[BINDLESS_TEXTURE_UAV_COUNT];

            core::IndexPool<BindlessHandle::Type, BINDLESS_BUFFER_UAV_COUNT>        m_bufferUAVIndexPool;
            Buffer *                                                                m_bufferUAV[BINDLESS_BUFFER_UAV_COUNT];
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