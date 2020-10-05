#pragma once

#include "core/Object/Object.h"
#include "core/Pool/IndexPool.h"
#include "BindlessTable_consts.h"
#include "graphics/driver/RootSignature/RootSignatureTableDesc.h"
#include "Shaders/driver/bindless.hlsli"

namespace vg::graphics::driver
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

            BindlessTextureSrvHandle allocBindlessTextureHandle(driver::Texture * _texture, ReservedSlot _reservedSlot);
            void freeBindlessTextureHandle(BindlessTextureSrvHandle & _handle);

            BindlessBufferSrvHandle allocBindlessBufferHandle(driver::Buffer * _buffer, ReservedSlot _reservedSlot);
            void freeBindlessBufferHandle(BindlessBufferSrvHandle & _handle);
            
        private:
            template <class T, class P> BindlessHandle allocBindlessHandle(T * _resource, ReservedSlot _reservedSlot, P & _pool, T ** _resources, uint _offset, uint _invalid);
            template <class T, class P> void freeBindlessHandle(BindlessHandle & _handle, P & _pool, T ** _resources, uint _offset, uint _invalid);

        private:
            RootSignatureTableDesc                                      m_tableDesc;

            core::IndexPool<BindlessHandle, bindless_texture_count>     m_textureSrvIndexPool;
            Texture *                                                   m_textureSrv[bindless_texture_count];

            core::IndexPool<BindlessHandle, bindless_buffer_count>      m_bufferSrvIndexPool;
            Buffer *                                                    m_bufferSrv[bindless_buffer_count];
        };
    }
}

#include VG_GFXAPI_HEADER(BindlessTable)

namespace vg::graphics::driver
{
    class BindlessTable : public VG_GFXAPI::BindlessTable
    {
        using super = VG_GFXAPI::BindlessTable;

    public:
        BindlessTable();
        ~BindlessTable();

        void init();

    private:
        driver::Texture *   m_defaultTexture = nullptr;
    };
}