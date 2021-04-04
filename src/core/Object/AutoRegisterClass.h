#pragma once

#include "core/IObjectFactory.h"

#define VG_AUTO_REGISTER_CLASS_EX(className, flags)     vg::core::AutoRegisterObjectClassHelper<className> autoRegister(#className, className::registerClass, flags)
#define VG_AUTO_REGISTER_CLASS(className)               VG_AUTO_REGISTER_CLASS_EX(className, vg::core::IObjectDescriptor::Flags::None)

namespace vg::core
{
    struct AutoRegisterClassInfo
    {
        using Func = bool(__cdecl *)(IObjectFactory &);

        const char *                name = nullptr;
        Func                        func = nullptr;
        IObjectDescriptor::Flags    flags = IObjectDescriptor::Flags::None;

        static bool push(const AutoRegisterClassInfo & _info);

        static bool registerClasses(IObjectFactory & _factory);
        static bool unregisterClasses(IObjectFactory & _factory);

        inline static core::vector<AutoRegisterClassInfo> * s_autoRegisterInfos = nullptr;  // use "construct-on-1st-use" idiom to avoid static initialization fiasco
    };

    class IObjectFactory;

    template <class T> class AutoRegisterObjectClassHelper
    {
    public:
        AutoRegisterObjectClassHelper(const char * _name, AutoRegisterClassInfo::Func _func, IObjectDescriptor::Flags _flags)
        {
            AutoRegisterClassInfo::push({ _name, _func, _flags });
        }

        ~AutoRegisterObjectClassHelper()
        {

        }
    };
}