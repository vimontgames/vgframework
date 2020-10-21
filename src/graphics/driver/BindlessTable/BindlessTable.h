#pragma once

#include "core/Object/Object.h"
#include "core/Pool/IndexPool.h"
#include "BindlessTable_consts.h"
#include "graphics/driver/RootSignature/RootSignatureTableDesc.h"

#include "Shaders/system/bindless.hlsli.h"

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

            BindlessTextureSrvHandle allocBindlessTextureHandle(driver::Texture * _texture, ReservedSlot _reservedSlot = ReservedSlot::None);
            void freeBindlessTextureHandle(BindlessTextureSrvHandle & _handle);

            BindlessBufferSrvHandle allocBindlessBufferHandle(driver::Buffer * _buffer, ReservedSlot _reservedSlot = ReservedSlot::None);
            void freeBindlessBufferHandle(BindlessBufferSrvHandle & _handle);
            
        private:
            template <class H, class T, class P> H allocBindlessHandle(T * _resource, ReservedSlot _reservedSlot, P & _pool, T ** _resources, core::uint _offset, core::uint _invalid);
            template <class H, class T, class P> void freeBindlessHandle(H & _handle, P & _pool, T ** _resources, core::uint _offset, core::uint _invalid);

        private:
            RootSignatureTableDesc                                              m_tableDesc;

            core::IndexPool<BindlessHandle::Type, bindless_texture_SRV_count>   m_textureSrvIndexPool;
            Texture *                                                           m_textureSrv[bindless_texture_SRV_count];

            core::IndexPool<BindlessHandle::Type, bindless_buffer_SRV_count>    m_bufferSrvIndexPool;
            Buffer *                                                            m_bufferSrv[bindless_buffer_SRV_count];
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