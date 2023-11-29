#pragma once

#include "shaders/system/table.hlsl.h"
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

    struct BindlessTextureHandle : public BindlessHandle
    {
        BindlessTextureHandle(Type _value = BINDLESS_TEXTURE_INVALID) : BindlessHandle(_value) { }
        static core::uint2 getValidRange() { return core::uint2(BINDLESS_TEXTURE_START, (BINDLESS_TEXTURE_START + BINDLESS_TEXTURE_COUNT - 1)); }
        bool isValid() const { return checkValidRange(getValidRange()); }
    };

    struct BindlessBufferHandle : public BindlessHandle
    {
        BindlessBufferHandle(Type _value = BINDLESS_BUFFER_INVALID) : BindlessHandle(_value) { }
        static core::uint2 getValidRange() { return core::uint2(BINDLESS_BUFFER_START, (BINDLESS_BUFFER_START + BINDLESS_BUFFER_COUNT - 1)); }
        bool isValid() const { return checkValidRange(getValidRange()); }
    };

    struct BindlessRWTextureHandle : public BindlessHandle
    {
        BindlessRWTextureHandle(Type _value = BINDLESS_RWTEXTURE_INVALID) : BindlessHandle(_value) {  }
        static core::uint2 getValidRange() { return core::uint2(BINDLESS_RWTEXTURE_START, (BINDLESS_RWTEXTURE_START + BINDLESS_RWTEXTURE_COUNT - 1)); }
        bool isValid() const { return checkValidRange(getValidRange()); }
    };

    struct BindlessRWBufferHandle : public BindlessHandle
    {
        BindlessRWBufferHandle(Type _value = BINDLESS_RWBUFFER_INVALID) : BindlessHandle(_value) { }
        static core::uint2 getValidRange() { return core::uint2(BINDLESS_RWBUFFER_START, (BINDLESS_RWBUFFER_START + BINDLESS_RWBUFFER_COUNT - 1)); }
        bool isValid() const { return checkValidRange(getValidRange()); }
    };

    // Allocate fixed Textures/Buffers SRVs/UAVs slots top-down (dynamic slots are allocated bottom-up)
    enum class ReservedSlot : core::u32
    {
        // Texture SRV
        ImGuiFontTexSrv         = RESERVEDSLOT_TEXSRV_IMGUIFONTTEX,

        // Buffer SRV
        ViewConstantsBufSrv     = RESERVEDSLOT_BUFSRV_VIEWCONSTANTS,
        SkinningMatricesBufSrv  = RESERVEDSLOT_BUFSRV_SKINNINGMATRICES,

        // Dynamic
        None                    = 0x80000000
    };
}