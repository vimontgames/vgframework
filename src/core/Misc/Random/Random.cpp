#include "core/Precomp.h"
#include "core/Misc/Random/Random.h"
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace vg::core;

VG_STATIC_ASSERT(RAND_MAX == 0x7FFF || RAND_MAX == 0xFFFF, "Unexpected RAND_MAX value");

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    void Random::init()
    {
        srand((uint)time(nullptr));
        s_initialized = true;
    }

    //--------------------------------------------------------------------------------------
    u16 Random::getRandomInRange(u16 _min, u16 _max)
    {
        if (!s_initialized)
            init();
        const u32 value = _min + (rand() % (_max - _min + 1));
        return value;
    }

    //--------------------------------------------------------------------------------------
    u32 Random::getRandomInRange(u32 _min, u32 _max)
    {
        if (!s_initialized)
            init();
        const u32 value = _min + ((rand()|(rand()<<16)) % (_max - _min + 1));
        return value;
    }

    //--------------------------------------------------------------------------------------
    float Random::getRandomInRange(float _min, float _max)
    {
        const float random = (float)(rand()) / (float)(RAND_MAX);
        return _min + random * (_max - _min);
    }

    //--------------------------------------------------------------------------------------
    core::float3 Random::getRandomInRange(const core::float3 & _min, const core::float3 & _max)
    {
        const float3 random = float3((float)rand(), (float)rand(), (float)rand()) / (float)(RAND_MAX);
        return _min + random * (_max - _min);
    }
}