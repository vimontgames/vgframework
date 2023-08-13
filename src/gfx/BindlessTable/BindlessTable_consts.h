#pragma once

#include "Shaders/system/bindless.hlsli.h"

namespace vg::gfx
{
    struct BindlessHandle
    {
        using Type = core::u16;
        BindlessHandle(Type _value) : value(_value) { }
        operator Type() const { return value; }

    private:
        Type value;
    };

    struct BindlessTextureSrvHandle : public BindlessHandle
    {
        BindlessTextureSrvHandle(Type _value = BINDLESS_TEXTURE_SRV_INVALID) : BindlessHandle(_value) { }
    };

    struct BindlessTextureUAVHandle : public BindlessHandle
    {
        BindlessTextureUAVHandle(Type _value = BINDLESS_TEXTURE_UAV_INVALID) : BindlessHandle(_value) {  }
    };

    struct BindlessBufferSrvHandle : public BindlessHandle
    {
        BindlessBufferSrvHandle(Type _value = BINDLESS_BUFFER_SRV_INVALID) : BindlessHandle(_value) { }
    };

    struct BindlessBufferUAVHandle : public BindlessHandle
    {
        BindlessBufferUAVHandle(Type _value = BINDLESS_BUFFER_UAV_INVALID) : BindlessHandle(_value) { }
    };

    enum ReservedSlot : core::u32
    {
        None = 0x80000000
    };
}