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
// 'Ex' versions of the functions take flags as parameters
//--------------------------------------------------------------------------------------

// Register property for a simple type (e.g. int, float, float4 ...)
#define registerPropertyEx(className, propertyName, displayName, flags)								        RegisterProperty(#className, #propertyName, (&((className*)(nullptr))->propertyName), displayName, flags)
#define registerProperty(className, propertyName, displayName)									            registerPropertyEx(className, propertyName, displayName, vg::core::IProperty::Flags::None)

// Register property for an Object or an Object container type
#define registerPropertyObjectEx(className, propertyName, displayName, flags)                               RegisterProperty(#className, #propertyName, (core::IObject*)offsetof(className, propertyName), displayName, flags);
#define registerPropertyObject(className, propertyName, displayName)                                        registerPropertyObjectEx(className, propertyName, displayName, vg::core::IProperty::Flags::None)

#define registerPropertyObjectVectorEx(className, propertyName, elementType, displayName, flags)            RegisterProperty(#className, #propertyName, sizeof(elementType), &((className*)nullptr)->propertyName, displayName, flags);
#define registerPropertyObjectVector(className, propertyName, elementType, displayName)                     registerPropertyObjectVectorEx(className, propertyName, elementType, displayName, vg::core::IProperty::Flags::None)

// Register property for an Object* or an Object* container  type
#define registerPropertyObjectPtrEx(className, propertyName, displayName, flags)                            RegisterProperty(#className, #propertyName, (core::IObject**)offsetof(className, propertyName), displayName, flags);
#define registerPropertyObjectPtr(className, propertyName, displayName)                                     registerPropertyObjectPtrEx(className, propertyName, displayName, vg::core::IProperty::Flags::None)

#define registerPropertyObjectPtrVectorEx(className, propertyName, displayName, flags)                      RegisterProperty(#className, #propertyName, (core::vector<core::IObject*>*)&((className*)nullptr)->propertyName, displayName, flags);
#define registerPropertyObjectPtrVector(className, propertyName, displayName)                               registerPropertyObjectPtrVectorEx(className, propertyName, displayName, vg::core::IProperty::Flags::None)

#define registerPropertyObjectPtrDictionaryEx(className, propertyName, displayName, flags)                  RegisterProperty(#className, #propertyName, (core::dictionary<core::IObject*>*)&((className*)nullptr)->propertyName, displayName, flags);
#define registerPropertyObjectPtrDictionary(className, propertyName, displayName)                           registerPropertyObjectPtrDictionaryEx(className, propertyName, displayName, vg::core::IProperty::Flags::None)

// Register property for an Resource* or an Resource* container  type
#define registerPropertyResourceEx(className, propertyName, displayName, flags)                             RegisterPropertyResource(#className, #propertyName, (core::IResource*) offsetof(className, propertyName), displayName, flags | vg::core::IProperty::Flags::Resource);
#define registerPropertyResource(className, propertyName, displayName)                                      registerPropertyResourceEx(className, propertyName, displayName, vg::core::IProperty::Flags::None)    

#define registerPropertyResourcePtrEx(className, propertyName, displayName, flags)                          RegisterPropertyResourcePtr(#className, #propertyName, (core::IResource**)offsetof(className, propertyName), displayName, flags | vg::core::IProperty::Flags::Resource);
#define registerPropertyResourcePtr(className, propertyName, displayName)                                   registerPropertyResourcePtrEx(className, propertyName, displayName, vg::core::IProperty::Flags::None)   

// Register property for a callback 
#define registerPropertyCallbackEx(className, funcName, displayName, flags)									RegisterProperty(#className, #funcName, funcName, displayName, flags)
#define registerPropertyCallback(className, funcName, displayName)	                                        registerPropertyCallbackEx(className, funcName, displayName, vg::core::IProperty::Flags::None)

// Register property for an enum
#define registerPropertyEnumEx(className, enumClassName, propertyName, displayName, flags)                  RegisterEnum(#className, #propertyName, (std::underlying_type_t<enumClassName>*)(&((className*)(nullptr))->propertyName), displayName, vg::core::EnumHelper<enumClassName>::getStaticCount(), vg::core::EnumHelper<enumClassName>::getStaticNames().c_str(), vg::core::EnumHelper<enumClassName>::getStaticValues().data(), flags);
#define registerPropertyEnum(className, enumClassName, propertyName, displayName)                           registerPropertyEnumEx(className, enumClassName, propertyName, displayName, vg::core::IProperty::Flags::None)

#define registerPropertyEnumBitfieldEx(className, enumClassName, propertyName, displayName, flags)          registerPropertyEnumEx(className, enumClassName, propertyName, displayName, vg::core::IProperty::Flags::Bitfield | flags)
#define registerPropertyEnumBitfield(className, enumClassName, propertyName, displayName)                   registerPropertyEnumEx(className, enumClassName, propertyName, displayName, vg::core::IProperty::Flags::Bitfield)

// Register property for fixed-size array using enum as index
#define registerPropertyEnumArray(className, elementType, enumClassName, propertyName, displayName, flags)  RegisterEnumArray(#className, #propertyName, (elementType*)(&((className*)(nullptr))->propertyName[0]), displayName, vg::core::EnumHelper<enumClassName>::getStaticCount(), sizeof(elementType), vg::core::EnumHelper<enumClassName>::getStaticNames().c_str(), (void*)vg::core::EnumHelper<enumClassName>::getStaticValues().data(), flags, vg::core::EnumHelper<enumClassName>::getSizeOfUnderlyingType());

//--------------------------------------------------------------------------------------
// Modify existing class properties macros
//--------------------------------------------------------------------------------------
#define setPropertyRangeHelper(className, propertyName, range)												GetPropertyByName(#propertyName)->setRange(range);

//--------------------------------------------------------------------------------------
// Misc
//--------------------------------------------------------------------------------------
#define registerResizeVectorFunc(className, resizeVectorFunc)                                               RegisterResizeVectorFunc(#className, resizeVectorFunc);
