#include "AutoRegisterClass.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    bool AutoRegisterClassInfo::push(const AutoRegisterClassInfo & _info)
    {
        if (!s_autoRegisterInfos)
        {
            s_autoRegisterInfos = new vector<AutoRegisterClassInfo>();
            //VG_DEBUGPRINT("New AutoRegisterClassInfo 0x%16p\n", s_autoRegisterInfos);
        }
        s_autoRegisterInfos->push_back(_info);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool AutoRegisterClassInfo::registerClasses(IFactory & _factory)
    {
        if (s_autoRegisterInfos)
        {
            const uint count = (uint)s_autoRegisterInfos->size();
            for (uint i = 0; i < count; ++i)
            {
                const AutoRegisterClassInfo & info = (*s_autoRegisterInfos)[i];
                info.func(_factory);
            }

            return true;
        }
        
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool AutoRegisterClassInfo::unregisterClasses()
    {
        if (s_autoRegisterInfos)
        {
            //VG_DEBUGPRINT("Delete AutoRegisterClassInfo 0x%16p\n", s_autoRegisterInfos);
            VG_SAFE_DELETE(s_autoRegisterInfos);
            return true;
        }

        return false;
    }
}