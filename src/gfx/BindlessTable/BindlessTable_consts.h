#pragma once

#include "shaders/system/bindless.hlsli.h"
#include "core/Math/Math.h"

namespace vg::gfx
{
    struct BindlessHandle
    {
        using Type = core::u16;
        BindlessHandle(Type _value) : value(_value) { }
        operator Type() const { return value; }

    protected:
        bool checkValidRange(core::uint2 _range) const
        {
            return core::within(value, (core::u16)_range.x, (core::u16)_range.y); // Last value in range stands for 'invalid' value
        }

    private:
        Type value;
    };

    struct BindlessTextureSrvHandle : public BindlessHandle
    {
        BindlessTextureSrvHandle(Type _value = BINDLESS_TEXTURE_SRV_INVALID) : BindlessHandle(_value) { }
        static core::uint2 getValidRange() { return core::uint2(BINDLESS_TEXTURE_SRV_START, (BINDLESS_TEXTURE_SRV_START + BINDLESS_TEXTURE_SRV_COUNT - 1)); }
        bool isValid() const { return checkValidRange(getValidRange()); }
    };

    struct BindlessTextureUAVHandle : public BindlessHandle
    {
        BindlessTextureUAVHandle(Type _value = BINDLESS_TEXTURE_UAV_INVALID) : BindlessHandle(_value) {  }
        static core::uint2 getValidRange() { return core::uint2(BINDLESS_TEXTURE_UAV_START, (BINDLESS_TEXTURE_UAV_START + BINDLESS_TEXTURE_UAV_COUNT - 1)); }
        bool isValid() const { return checkValidRange(getValidRange()); }
    };

    struct BindlessBufferSrvHandle : public BindlessHandle
    {
        BindlessBufferSrvHandle(Type _value = BINDLESS_BUFFER_SRV_INVALID) : BindlessHandle(_value) { }
        static core::uint2 getValidRange() { return core::uint2(BINDLESS_BUFFER_SRV_START, (BINDLESS_BUFFER_SRV_START + BINDLESS_BUFFER_SRV_COUNT - 1)); }
        bool isValid() const { return checkValidRange(getValidRange()); }
    };

    struct BindlessBufferUAVHandle : public BindlessHandle
    {
        BindlessBufferUAVHandle(Type _value = BINDLESS_BUFFER_UAV_INVALID) : BindlessHandle(_value) { }
        static core::uint2 getValidRange() { return core::uint2(BINDLESS_BUFFER_UAV_START, (BINDLESS_BUFFER_UAV_START + BINDLESS_BUFFER_UAV_COUNT - 1)); }
        bool isValid() const { return checkValidRange(getValidRange()); }
    };

    // Allocate fixed Textures/Buffers SRVs/UAVs slots top-down (dynamic slots are allocated bottom-up)
    enum class ReservedSlot : core::u32
    {
        // Texture SRV
        ImGuiFontTexSrv         = RESERVEDSLOT_TEXSRV_IMGUIFONTTEX,

        // Buffer SRV
        ViewConstantsBufSrv     = RESERVEDSLOT_BUFSRV_VIEWCONSTANTS,

        // Dynamic
        None                    = 0x80000000
    };
}