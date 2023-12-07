#pragma once

#include "core/IFactory.h"

//--------------------------------------------------------------------------------------
// Register object class macros
//--------------------------------------------------------------------------------------
#define VG_REGISTER_OBJECT_CLASS_EX(className, displayName, flags)                      vg::core::AutoRegisterObjectClassHelper<className> autoRegister##className(#className, className::registerClass);   \
                                                                                        bool className::registerClass(vg::core::IFactory & _factory)                                                        \
                                                                                        {                                                                                                                   \
                                                                                            if (vg::core::IClassDesc * desc = _factory.registerClassHelper(className, displayName, flags))                  \
                                                                                            {                                                                                                               \
                                                                                                registerProperties(*desc);                                                                                  \
                                                                                                return true;                                                                                                \
                                                                                            }                                                                                                               \
                                                                                            return false;                                                                                                   \
                                                                                        }

#define VG_REGISTER_OBJECT_CLASS(className, displayName)                                VG_REGISTER_OBJECT_CLASS_EX(className, displayName, vg::core::IClassDesc::Flags::None)
#define VG_REGISTER_RESOURCE_CLASS(className, displayName)                              VG_REGISTER_OBJECT_CLASS_EX(className, displayName, vg::core::IClassDesc::Flags::Resource)

#define VG_REGISTER_COMPONENT_CLASS(className, displayName, category, description)      vg::core::AutoRegisterObjectClassHelper<className> autoRegister##className(#className, className::registerClass);                   \
                                                                                        bool className::registerClass(vg::core::IFactory & _factory)                                                                        \
                                                                                        {                                                                                                                                   \
                                                                                            if (vg::core::IClassDesc * desc = _factory.registerClassHelper(className, displayName, vg::core::IClassDesc::Flags::Component)) \
                                                                                            {                                                                                                                               \
                                                                                                registerProperties(*desc);                                                                                                  \
                                                                                                desc->SetCategory(category);                                                                                                \
                                                                                                desc->SetDescription(description);                                                                                          \
                                                                                                return true;                                                                                                                \
                                                                                            }                                                                                                                               \
                                                                                            return false;                                                                                                                   \
                                                                                        }
namespace vg::core
{
    struct AutoRegisterClassInfo
    {
        using Func = bool(__cdecl *)(IFactory &);

        const char *                name = nullptr;
        Func                        func = nullptr;

        static bool push(const AutoRegisterClassInfo & _info);

        static bool registerClasses(IFactory & _factory);
        static bool unregisterClasses(IFactory & _factory);

        inline static core::vector<AutoRegisterClassInfo> * s_autoRegisterInfos = nullptr;  // use "construct-on-1st-use" idiom to avoid static initialization fiasco
    };

    class IFactory;

    template <class T> class AutoRegisterObjectClassHelper
    {
    public:
        AutoRegisterObjectClassHelper(const char * _name, AutoRegisterClassInfo::Func _func)
        {
            AutoRegisterClassInfo::push({ _name, _func });
        }

        ~AutoRegisterObjectClassHelper()
        {

        }
    };
}