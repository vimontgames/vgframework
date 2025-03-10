#include "core.tests/Precomp.h"

#include <core/Math/Math.h>

namespace vg::core::tests
{
    //--------------------------------------------------------------------------------------
    TEST(Math, ctz_u32)
    {
        EXPECT_EQ(ctz(u32(0x00000001)), 0);
        EXPECT_EQ(ctz(u32(0x00000002)), 1);
        EXPECT_EQ(ctz(u32(0x00000004)), 2);
        EXPECT_EQ(ctz(u32(0x00000008)), 3);
        EXPECT_EQ(ctz(u32(0x00000010)), 4);
        EXPECT_EQ(ctz(u32(0x80000000)), 31);
        EXPECT_EQ(ctz(u32(0x40000000)), 30);
        EXPECT_EQ(ctz(u32(0x20000000)), 29);
        EXPECT_EQ(ctz(u32(0x10000000)), 28);
    }

    //--------------------------------------------------------------------------------------
    TEST(Math, ctz_u32_zero)
    {
        EXPECT_EQ(ctz(u32(0x00000000)), 32);
    }

    //--------------------------------------------------------------------------------------
    TEST(Math, ctz_u64)
    {
        EXPECT_EQ(ctz(u64(0x0000000000000001)), 0);
        EXPECT_EQ(ctz(u64(0x0000000000000002)), 1);
        EXPECT_EQ(ctz(u64(0x0000000000000004)), 2);
        EXPECT_EQ(ctz(u64(0x0000000000000008)), 3);
        EXPECT_EQ(ctz(u64(0x0000000000000010)), 4);
        EXPECT_EQ(ctz(u64(0x8000000000000000)), 63);
        EXPECT_EQ(ctz(u64(0x4000000000000000)), 62);
        EXPECT_EQ(ctz(u64(0x2000000000000000)), 61);
        EXPECT_EQ(ctz(u64(0x1000000000000000)), 60);
    }

    //--------------------------------------------------------------------------------------
    TEST(Math, ctz_u64_zero)
    {
        EXPECT_EQ(ctz(u64(0x0000000000000000)), 64);
    }

    //--------------------------------------------------------------------------------------
    TEST(Math, clz_u32)
    {
        EXPECT_EQ(clz(u32(0x00000001)), 31);
        EXPECT_EQ(clz(u32(0x00000002)), 30);
        EXPECT_EQ(clz(u32(0x00000004)), 29);
        EXPECT_EQ(clz(u32(0x00000008)), 28);
        EXPECT_EQ(clz(u32(0x00000010)), 27);
        EXPECT_EQ(clz(u32(0x80000000)), 0);
        EXPECT_EQ(clz(u32(0x40000000)), 1);
        EXPECT_EQ(clz(u32(0x20000000)), 2);
        EXPECT_EQ(clz(u32(0x10000000)), 3);
    }

    //--------------------------------------------------------------------------------------
    TEST(Math, clz_u32_zero)
    {
        EXPECT_EQ(clz(u32(0x00000000)), 32);
    }

    //--------------------------------------------------------------------------------------
    TEST(Math, clz_u64)
    {
        EXPECT_EQ(clz(u64(0x0000000000000001)), 63);
        EXPECT_EQ(clz(u64(0x0000000000000002)), 62);
        EXPECT_EQ(clz(u64(0x0000000000000004)), 61);
        EXPECT_EQ(clz(u64(0x0000000000000008)), 60);
        EXPECT_EQ(clz(u64(0x0000000000000010)), 59);
        EXPECT_EQ(clz(u64(0x8000000000000000)), 0);
        EXPECT_EQ(clz(u64(0x4000000000000000)), 1);
        EXPECT_EQ(clz(u64(0x2000000000000000)), 2);
        EXPECT_EQ(clz(u64(0x1000000000000000)), 3);
    }

    //--------------------------------------------------------------------------------------
    TEST(Math, clz_u64_zero)
    {
        EXPECT_EQ(clz(u64(0x0000000000000000)), 64);
    }

    //--------------------------------------------------------------------------------------
    TEST(Math, float4)
    {
        const float4 vec1234 = float4(1, 2, 3, 4);
        const float4 vec0001 = float4(0, 0, 0, 1);

        EXPECT_EQ(dot(vec1234, vec0001), 4);

        // We can't use 'EXPECT_EQ(vec1234 * vec0001, float4(0, 0, 0, 4));' here because 
        // testing SIMD vector equality returns a vector, not a bool. 
        // But we can test each component separately ...
        EXPECT_EQ((vec1234 * vec0001).x, 0);
        EXPECT_EQ((vec1234 * vec0001).y, 0);
        EXPECT_EQ((vec1234 * vec0001).z, 0);
        EXPECT_EQ((vec1234 * vec0001).w, 4);

        // ... or use EXPECT_TRUE with any/all
        EXPECT_TRUE(all(vec1234 * vec0001 == float4(0, 0, 0, 4)));
    }
}

