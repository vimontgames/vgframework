#pragma once

namespace vg::core
{
    class Random
    {
    public:
        static void init();
        static core::u16 getRandomInRange(core::u16 _min, core::u16 _max);
        static core::u32 getRandomInRange(core::u32 _min, core::u32 _max);

    private:
        static inline bool s_initialized = false;
    };
}