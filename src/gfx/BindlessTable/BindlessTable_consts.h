#pragma once

#include "shaders/system/bindless.hlsli.h"

namespace vg::gfx
{
    struct BindlessHandle
    {
        using Type = core::u16;
        BindlessHandle(Type _value) : value(_value) { }
        operator Type() const { return value; }

    protected:
        bool inRange(Type _begin, Type _end) const
        {
            return value >= _begin && value < _end;
        }

    private:
        Type value;
    };

    struct BindlessTextureSrvHandle : public BindlessHandle
    {
        BindlessTextureSrvHandle(Type _value = BINDLESS_TEXTURE_SRV_INVALID) : BindlessHandle(_value) { }
        bool isValid() const { return inRange(BINDLESS_TEXTURE_SRV_START, (BINDLESS_TEXTURE_SRV_START + BINDLESS_TEXTURE_SRV_COUNT - 1)); }
    };

    struct BindlessTextureUAVHandle : public BindlessHandle
    {
        BindlessTextureUAVHandle(Type _value = BINDLESS_TEXTURE_UAV_INVALID) : BindlessHandle(_value) {  }
        bool isValid() const { return inRange(BINDLESS_TEXTURE_UAV_START, (BINDLESS_TEXTURE_UAV_START + BINDLESS_TEXTURE_UAV_COUNT - 1)); }
    };

    struct BindlessBufferSrvHandle : public BindlessHandle
    {
        BindlessBufferSrvHandle(Type _value = BINDLESS_BUFFER_SRV_INVALID) : BindlessHandle(_value) { }
        bool isValid() const { return inRange(BINDLESS_BUFFER_SRV_START, (BINDLESS_BUFFER_SRV_START + BINDLESS_BUFFER_SRV_COUNT - 1)); }
    };

    struct BindlessBufferUAVHandle : public BindlessHandle
    {
        BindlessBufferUAVHandle(Type _value = BINDLESS_BUFFER_UAV_INVALID) : BindlessHandle(_value) { }
        bool isValid() const { return inRange(BINDLESS_BUFFER_UAV_START, (BINDLESS_BUFFER_UAV_START + BINDLESS_BUFFER_UAV_COUNT - 1)); }
    };

    enum ReservedSlot : core::u32
    {
        None = 0x80000000
    };
}