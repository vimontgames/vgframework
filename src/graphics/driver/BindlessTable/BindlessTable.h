#pragma once

#include "core/Object/Object.h"
#include "core/Pool/IndexPool.h"
#include "BindlessTable_consts.h"
#include "graphics/driver/RootSignature/RootSignatureTableDesc.h"

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

            BindlessTextureHandle allocBindlessTextureHandle(driver::Texture * _texture, ReservedSlot _reservedSlot);
            void freeBindlessTextureHandle(BindlessTextureHandle & _handle);

            BindlessBufferHandle allocBindlessBufferHandle(driver::Buffer * _buffer, ReservedSlot _reservedSlot);
            void freeBindlessBufferHandle(BindlessBufferHandle & _handle);

        protected:
            BindlessHandle allocBindlessHandle(ReservedSlot _reservedSlot);
            void freeBindlessHandle(BindlessBufferHandle & _handle, const BindlessHandle & _invalidHandleValue);
        
        private:
            enum class Flags : core::u8
            {
                Texture = 0x01,
                Buffer  = 0x02,
                UAV     = 0x04
            };

            struct SlotDesc
            {
                SlotDesc() : 
                    flags((Flags)0), 
                    ptr(nullptr)
                {

                }

                Flags flags;
                union
                {
                    driver::Texture * texture;
                    driver::Buffer * buffer;
                    void * ptr;
                };
            };

            RootSignatureTableDesc                                  m_tableDesc;
            core::IndexPool<BindlessHandle, max_bindless_elements>  m_srvIndexPool;
            SlotDesc                                                m_slotDesc[max_bindless_elements];
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