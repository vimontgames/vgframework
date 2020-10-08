#define VG_ENABLE_OPTICK

#ifdef VG_ENABLE_OPTICK
#include "optick/src/optick.h"
#endif

namespace vg::graphics::driver
{
    class CommandList;

    class Profiler
    {
    public:
        static void init();
        static void deinit();
        static void setCommandList(CommandList * _cmdList);
        static void swap();
    };
}