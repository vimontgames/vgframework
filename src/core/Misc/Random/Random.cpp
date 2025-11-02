#include "core/Precomp.h"
#include "core/Misc/Random/Random.h"
#include "core/Kernel.h"
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace vg::core;

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    void Random::init()
    {
        const auto t = static_cast<uint32_t>(time(nullptr));
        VG_INFO("[Random] Use random seed 0x%08X", t);

        s_rng.seed(t);
        s_initialized = true;
    }

    //--------------------------------------------------------------------------------------
    u16 Random::getRandomInRange(u16 _min, u16 _max)
    {
        if (!s_initialized)
            init();

        std::uniform_int_distribution<u16> dist(_min, _max);
        return dist(s_rng);
    }

    //--------------------------------------------------------------------------------------
    u32 Random::getRandomInRange(u32 _min, u32 _max)
    {
        if (!s_initialized)
            init();

        std::uniform_int_distribution<u32> dist(_min, _max);
        return dist(s_rng);
    }

    //--------------------------------------------------------------------------------------
    float Random::getRandomInRange(float _min, float _max)
    {
        if (!s_initialized)
            init();

        std::uniform_real_distribution<float> dist(_min, _max);
        return dist(s_rng);
    }

    //--------------------------------------------------------------------------------------
    core::float3 Random::getRandomInRange(const core::float3 & _min, const core::float3 & _max)
    {
        if (!s_initialized)
            init();

        std::uniform_real_distribution<float> distX(_min.x, _max.x);
        std::uniform_real_distribution<float> distY(_min.y, _max.y);
        std::uniform_real_distribution<float> distZ(_min.z, _max.z);

        return float3(distX(s_rng), distY(s_rng), distZ(s_rng));
    }
}