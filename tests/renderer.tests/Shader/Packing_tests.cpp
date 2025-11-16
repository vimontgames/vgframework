#include "renderer.tests/Precomp.h"

#include "system/packing.hlsli"

namespace vg::core::tests
{
    //--------------------------------------------------------------------------------------
    TEST(Packing, RGBA)
    {
        EXPECT_EQ(packRGBA8(float4(1.0f, 1.0f, 1.0f, 1.0f)), 0xFFFFFFFF);
        EXPECT_EQ(unpackRGBA8(0xFFFFFFFF).x, 1.0f);
        EXPECT_EQ(unpackRGBA8(0xFFFFFFFF).y, 1.0f);
        EXPECT_EQ(unpackRGBA8(0xFFFFFFFF).z, 1.0f);
        EXPECT_EQ(unpackRGBA8(0xFFFFFFFF).w, 1.0f);

        EXPECT_EQ(packRGBA8(float4(0.0f, 0.0f, 0.0f, 1.0f)), 0xFF000000);
        EXPECT_EQ(unpackRGBA8(0xFF000000).x, 0.0f);
        EXPECT_EQ(unpackRGBA8(0xFF000000).y, 0.0f);
        EXPECT_EQ(unpackRGBA8(0xFF000000).z, 0.0f);
        EXPECT_EQ(unpackRGBA8(0xFF000000).w, 1.0f);

        EXPECT_EQ(packR8(0x12345678, 0x42), 0x12345642);
        EXPECT_EQ(unpackR8(0x12345642), 0x42);

        EXPECT_EQ(packG8(0x00000000, 0x42), 0x00004200);
        EXPECT_EQ(unpackG8(0x00004200), 0x42);

        EXPECT_EQ(packB8(0xFFFFFFFF, 0x42), 0xFF42FFFF);
        EXPECT_EQ(unpackB8(0xFF42FFFF), 0x42);

        EXPECT_EQ(packA8(0xaaaaaaaa, 0x42), 0x42aaaaaa);
        EXPECT_EQ(unpackA8(0x42aaaaaa), 0x42);
    } 

    //--------------------------------------------------------------------------------------
    TEST(Packing, Short)
    {
        EXPECT_EQ(packUint16(uint2(0x1234, 0x5678)), 0x56781234);
        EXPECT_EQ(unpackUint16(0x56781234).x, 0x1234);
        EXPECT_EQ(unpackUint16(0x56781234).y, 0x5678);

        EXPECT_EQ(packUint16low(0xFFFFFFFF, 0x1234), 0xFFFF1234);
        EXPECT_EQ(unpackUint16low(0xFFFF1234), 0x1234);
        EXPECT_EQ(packUint16high(0xFFFFFFFF, 0xABCD), 0xABCDFFFF);
        EXPECT_EQ(unpackUint16high(0xABCDFFFF), 0xABCD);
    }

    //--------------------------------------------------------------------------------------
    TEST(Packing, Uint)
    {
        EXPECT_EQ(packUint(0x12345678, 0x42, 0xFF, 16), 0x12425678);
        EXPECT_EQ(unpackUint(0x12425678, 0xFF, 16), 0x42);

        EXPECT_EQ(packUint(0x12345678, 15, 0xF, 0), 0x1234567F);
        EXPECT_EQ(unpackUint(0x1234567F, 0xF, 0), 15);
    }
}

