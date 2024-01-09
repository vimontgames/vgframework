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
        explicit BindlessTextureHandle(Type _value = BINDLESS_TEXTURE_INVALID) : 
            BindlessHandle(_value) 
        { 
        }
        static core::uint2 getValidRange() { return core::uint2(BINDLESS_TEXTURE_START, (BINDLESS_TEXTURE_START + BINDLESS_TEXTURE_COUNT - 1)); }
        bool isValid() const { return checkValidRange(getValidRange()); }
    };

    struct BindlessBufferHandle : public BindlessHandle
    {
        explicit BindlessBufferHandle(Type _value = BINDLESS_BUFFER_INVALID) :
            BindlessHandle(_value) 
        {
        }
        static core::uint2 getValidRange() { return core::uint2(BINDLESS_BUFFER_START, (BINDLESS_BUFFER_START + BINDLESS_BUFFER_COUNT - 1)); }
        bool isValid() const { return checkValidRange(getValidRange()); }
    };

    struct BindlessRWTextureHandle : public BindlessHandle
    {
        explicit BindlessRWTextureHandle(Type _value = BINDLESS_RWTEXTURE_INVALID) :
            BindlessHandle(_value) 
        {  
        }
        static core::uint2 getValidRange() { return core::uint2(BINDLESS_RWTEXTURE_START, (BINDLESS_RWTEXTURE_START + BINDLESS_RWTEXTURE_COUNT - 1)); }
        bool isValid() const { return checkValidRange(getValidRange()); }
    };

    struct BindlessRWBufferHandle : public BindlessHandle
    {
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

    // Allocate fixed Textures/Buffers SRVs/UAVs slots top-down (dynamic slots are allocated bottom-up)
    enum class ReservedSlot : core::u32
    {
        // Texture SRV
        DefaultPBRTexSrv        = RESERVEDSLOT_TEXSRV_DEFAULT_PBR,
        DefaultNormalTexSrv     = RESERVEDSLOT_TEXSRV_DEFAULT_NORMAL,
        DefaultAlbedoTexSrv     = RESERVEDSLOT_TEXSRV_DEFAULT_ALBEDO,
        ImGuiFontTexSrv         = RESERVEDSLOT_TEXSRV_IMGUIFONTTEX,

        // Buffer SRV
        InstanceDataBufSrv      = RESERVEDSLOT_BUFSRV_INSTANCEDATA,
        SkinningMatricesBufSrv  = RESERVEDSLOT_BUFSRV_SKINNINGMATRICES,
        LightsConstantBufSrv    = RESERVEDSLOT_BUFSRV_LIGHTSCONSTANTS,
        ViewConstantsBufSrv     = RESERVEDSLOT_BUFSRV_VIEWCONSTANTS,

        // RWBuffer 
        GPUDebugBufRW           = RESERVEDSLOT_RWBUFFER_GPUDEBUG,
        
        // Dynamic
        None                    = 0x80000000
    };
}