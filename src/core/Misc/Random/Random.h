#pragma once
#include <random>

namespace vg::core
{
    class Random
    {
    public:
        static void init();
        static core::u16 getRandomInRange(core::u16 _min, core::u16 _max);
        static core::u32 getRandomInRange(core::u32 _min, core::u32 _max);
        static float getRandomInRange(float _min, float _max);
        static core::float3 getRandomInRange(const core::float3 & _min, const core::float3 & _max);

    private:
        static inline bool s_initialized = false;
        static inline std::mt19937 s_rng;
    };
}