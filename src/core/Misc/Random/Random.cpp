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
}