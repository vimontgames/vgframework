#pragma once

#include "shaders/system/table.hlsl.h"
#include "core/Math/Math.h"

namespace vg::gfx
{
    struct BindlessHandle
    {
        using Type = core::u16;
        BindlessHandle(Type _value) : value(_value) 
        {
        }
        operator Type() const { return value; }

    protected:
        bool checkValidRange(core::uint2 _range) const
        {
            return core::within(value, (core::u16)_range.x, (core::u16)_range.y); // Last value in range stands for 'invalid' value
        }

    private:
        Type value;
    };

    struct BindlessTextureHandle : public BindlessHandle
    {
        BindlessTextureHandle(ReservedSlot _slot) :
            BindlessHandle((Type)_slot)
        {
            VG_ASSERT((core::uint)ReservedSlot::FirstTexture <= (core::uint)_slot && (core::uint)_slot <= (core::uint)ReservedSlot::LastTexture, "Texture %u (0x%08X) is not in the [%u..%u] range", (core::uint)_slot, (core::uint)_slot, (core::uint)ReservedSlot::FirstTexture, (core::uint)ReservedSlot::LastTexture);
        }

        explicit BindlessTextureHandle(Type _value = BINDLESS_TEXTURE_INVALID) : 
            BindlessHandle(_value) 
        { 
        }
        static core::uint2 getValidRange() { return core::uint2(BINDLESS_TEXTURE_START, (BINDLESS_TEXTURE_START + BINDLESS_TEXTURE_COUNT - 1)); }
        bool isValid() const { return checkValidRange(getValidRange()); }
    };

    struct BindlessBufferHandle : public BindlessHandle
    {
        BindlessBufferHandle(ReservedSlot _slot) :
            BindlessHandle((Type)_slot)
        {
            VG_ASSERT((core::uint)ReservedSlot::FirstBuffer <= (core::uint)_slot && (core::uint)_slot <= (core::uint)ReservedSlot::LastBuffer, "Buffer %u (0x%08X) is not in the [%u..%u] range", (core::uint)_slot, (core::uint)_slot, (core::uint)ReservedSlot::FirstBuffer, (core::uint)ReservedSlot::LastBuffer);
        }

        explicit BindlessBufferHandle(Type _value = BINDLESS_BUFFER_INVALID) :
            BindlessHandle(_value) 
        {
        }
        static core::uint2 getValidRange() { return core::uint2(BINDLESS_BUFFER_START, (BINDLESS_BUFFER_START + BINDLESS_BUFFER_COUNT - 1)); }
        bool isValid() const { return checkValidRange(getValidRange()); }
    };

    struct BindlessRWTextureHandle : public BindlessHandle
    {
        BindlessRWTextureHandle(ReservedSlot _slot) :
            BindlessHandle((Type)_slot)
        {
            VG_ASSERT((core::uint)ReservedSlot::FirstTexture <= (core::uint)_slot && (core::uint)_slot <= (core::uint)ReservedSlot::LastRWTexture, "RWTexture %u (0x%08X) is not in the [%u..%u] range", (core::uint)_slot, (core::uint)_slot, (core::uint)ReservedSlot::FirstTexture, (core::uint)ReservedSlot::LastRWTexture);
        }

        explicit BindlessRWTextureHandle(Type _value = BINDLESS_RWTEXTURE_INVALID) :
            BindlessHandle(_value) 
        {  
        }
        static core::uint2 getValidRange() { return core::uint2(BINDLESS_RWTEXTURE_START, (BINDLESS_RWTEXTURE_START + BINDLESS_RWTEXTURE_COUNT - 1)); }
        bool isValid() const { return checkValidRange(getValidRange()); }
    };

    struct BindlessRWBufferHandle : public BindlessHandle
    {
        BindlessRWBufferHandle(ReservedSlot _slot) :
            BindlessHandle((Type)_slot)
        {
            VG_ASSERT((core::uint)ReservedSlot::FirstRWBuffer <= (core::uint)_slot && (core::uint)_slot <= (core::uint)ReservedSlot::LastRWBuffer, "RWBuffer %u (0x%08X) is not in the [%u..%u] range", (core::uint)_slot, (core::uint)_slot, (core::uint)ReservedSlot::FirstRWBuffer, (core::uint)ReservedSlot::LastRWBuffer);
        }

        explicit BindlessRWBufferHandle(Type _value = BINDLESS_RWBUFFER_INVALID) :
            BindlessHandle(_value) 
        { 
        }
        static core::uint2 getValidRange() { return core::uint2(BINDLESS_RWBUFFER_START, (BINDLESS_RWBUFFER_START + BINDLESS_RWBUFFER_COUNT - 1)); }
        bool isValid() const { return checkValidRange(getValidRange()); }
    };

    struct BindlessTLASHandle : public BindlessHandle
    {
        explicit BindlessTLASHandle(Type _value = BINDLESS_TLAS_INVALID) :
            BindlessHandle(_value) 
        { 
        }
        static core::uint2 getValidRange() { return core::uint2(BINDLESS_TLAS_START, (BINDLESS_TLAS_START + BINDLESS_TLAS_COUNT - 1)); }
        bool isValid() const { return checkValidRange(getValidRange()); }
    };
}