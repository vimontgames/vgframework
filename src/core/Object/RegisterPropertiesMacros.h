#pragma once

//--------------------------------------------------------------------------------------
// Register object class macros
//--------------------------------------------------------------------------------------
#define registerClassHelper_NoCTor(className, displayName, flags)											registerClass(#className, displayName, flags, sizeof(className), [](const vg::core::string & _name, vg::core::IObject * _parent) { return nullptr; })
#define registerClassHelper(className, displayName, flags)                                                  registerClass(#className, displayName, flags, sizeof(className), [](const vg::core::string & _name, vg::core::IObject * _parent) { auto newObj = new className(_name, _parent); VG_ASSERT(nullptr != dynamic_cast<vg::core::IObject*>(newObj)); return dynamic_cast<vg::core::IObject*>(newObj); }) // 'dynamic_cast' should not be necessary but the cast is present to workaround weird Lambda to std::function conversion compilation issue  
#define registerClassSingletonHelper(className, displayName, flags)											registerSingletonClass(#className, displayName, flags | vg::core::IClassDesc::Flags::Singleton, sizeof(className), [](){ return className::get(); } )
#define registerPlugin(className, displayName)											                    registerSingletonClass(#className, displayName, vg::core::IClassDesc::Flags::Singleton | vg::core::IClassDesc::Flags::Plugin, sizeof(className), [](){ return className::get(); } )

//--------------------------------------------------------------------------------------
// Register object properties macros
// Ex versions take flags as parameters
//--------------------------------------------------------------------------------------
#define registerPropertyHelper(className, propertyName, displayName)									    RegisterProperty(#className, #propertyName, (&((className*)(nullptr))->propertyName), displayName, vg::core::IProperty::Flags::None)
#define registerPropertyHelperEx(className, propertyName, displayName, flags)								RegisterProperty(#className, #propertyName, (&((className*)(nullptr))->propertyName), displayName, flags)

#define registerPropertyObjectHelper(className, propertyName, displayName, flags)                           RegisterProperty(#className, #propertyName, (core::IObject*)offsetof(className, propertyName), displayName, flags);
#define registerPropertyObjectRefHelper(className, propertyName, displayName, flags)                        RegisterProperty(#className, #propertyName, (core::IObject**)offsetof(className, propertyName), displayName, flags);
#define registerPropertyObjectVectorHelper(className, propertyName, elementType, displayName, flags)        RegisterProperty(#className, #propertyName, sizeof(elementType), &((className*)nullptr)->propertyName, displayName, flags);
#define registerPropertyObjectRefVectorHelper(className, propertyName, displayName, flags)                  RegisterProperty(#className, #propertyName, (core::vector<core::IObject*>*)&((className*)nullptr)->propertyName, displayName, flags);
#define registerPropertyObjectRefDictionaryHelper(className, propertyName, displayName, flags)              RegisterProperty(#className, #propertyName, (core::dictionary<core::IObject*>*)&((className*)nullptr)->propertyName, displayName, flags);

#define registerPropertyResourceHelper(className, propertyName, displayName, flags)                         RegisterPropertyResource(#className, #propertyName, (core::IResource*) offsetof(className, propertyName), displayName, flags | vg::core::IProperty::Flags::Resource);
#define registerPropertyResourceRefHelper(className, propertyName, displayName, flags)                      RegisterPropertyResourcePtr(#className, #propertyName, (core::IResource**)offsetof(className, propertyName), displayName, flags | vg::core::IProperty::Flags::Resource);

#define registerCallbackHelper(className, funcName, displayName, flags)										RegisterProperty(#className, #funcName, funcName, displayName, flags)

#define registerPropertyEnumWithFlags(className, enumClassName, propertyName, displayName, flags)           RegisterEnum(#className, #propertyName, (std::underlying_type_t<enumClassName>*)(&((className*)(nullptr))->propertyName), displayName, vg::core::EnumHelper<enumClassName>::getStaticCount(), vg::core::EnumHelper<enumClassName>::getStaticNames().c_str(), vg::core::EnumHelper<enumClassName>::getStaticValues().data(), flags);
#define registerPropertyEnum(className, enumClassName, propertyName, displayName)                           registerPropertyEnumWithFlags(className, enumClassName, propertyName, displayName, vg::core::IProperty::Flags::None)
#define registerPropertyEnumBitfield(className, enumClassName, propertyName, displayName)                   registerPropertyEnumWithFlags(className, enumClassName, propertyName, displayName, vg::core::IProperty::Flags::Bitfield)
#define registerPropertyEnumBitfieldWithFlags(className, enumClassName, propertyName, displayName, flags)   registerPropertyEnumWithFlags(className, enumClassName, propertyName, displayName, vg::core::IProperty::Flags::Bitfield | flags)

#define registerPropertyEnumArray(className, elementType, enumClassName, propertyName, displayName, flags)  RegisterEnumArray(#className, #propertyName, (elementType*)(&((className*)(nullptr))->propertyName[0]), displayName, vg::core::EnumHelper<enumClassName>::getStaticCount(), sizeof(elementType), vg::core::EnumHelper<enumClassName>::getStaticNames().c_str(), (void*)vg::core::EnumHelper<enumClassName>::getStaticValues().data(), flags, vg::core::EnumHelper<enumClassName>::getSizeOfUnderlyingType());

//--------------------------------------------------------------------------------------
// Modify existing class properties macros
//--------------------------------------------------------------------------------------
#define setPropertyRangeHelper(className, propertyName, range)												GetPropertyByName(#propertyName)->setRange(range);

//--------------------------------------------------------------------------------------
// Misc
//--------------------------------------------------------------------------------------
#define propertyOffset(typeName, className, propertyName)													(typeName*)&((className*)(nullptr))->propertyName
#define registerResizeVectorFunc(className, resizeVectorFunc)                                               RegisterResizeVectorFunc(#className, resizeVectorFunc);
