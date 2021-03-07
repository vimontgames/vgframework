#include "ObjectFactory.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    const char * ObjectFactory::ClassProperty::getName() const
    {
        return name;
    }

    //--------------------------------------------------------------------------------------
    const char * ObjectFactory::ClassProperty::getDisplayName() const
    {
        if (displayName)
            return displayName;
        else 
            return name;
    }

    //--------------------------------------------------------------------------------------
    IPropertyDescriptor::Type ObjectFactory::ClassProperty::getType() const
    {
        return type;
    }

    //--------------------------------------------------------------------------------------
    uint_ptr ObjectFactory::ClassProperty::getOffset() const
    {
        return offset;
    }

    //--------------------------------------------------------------------------------------
    void ObjectFactory::ClassDesc::registerProperty(const char * _propertyName, bool * _offset, const char * _displayName)
    {
        registerClassMemberT(_propertyName, _offset, _displayName);
    }

    //--------------------------------------------------------------------------------------
    void ObjectFactory::ClassDesc::registerProperty(const char * _propertyName, core::float4 * _offset, const char * _displayName)
    {
        registerClassMemberT(_propertyName, _offset, _displayName);
    }

    //--------------------------------------------------------------------------------------
    const char * ObjectFactory::ClassDesc::getClassName() const
    {
        return name;
    }

    //--------------------------------------------------------------------------------------
    uint ObjectFactory::ClassDesc::getPropertyCount() const
    {
        return (uint)properties.size();
    }

    //--------------------------------------------------------------------------------------
    const IPropertyDescriptor * ObjectFactory::ClassDesc::getProperty(uint _index) const
    {
        return &properties[_index];
    }

    //--------------------------------------------------------------------------------------
    // Returned pointer shall not be stored but used immediatly ;)
    //--------------------------------------------------------------------------------------
    IObjectDescriptor & ObjectFactory::registerClass(const char * _className, CreateFunc _createFunc)
    {
        VG_ASSERT(nullptr == getClassDescriptor(_className), "Class \"%s\" is already registered", _className);

        ClassDesc classDesc;
                  classDesc.name = _className;
                  classDesc.createFunc = _createFunc;

        m_classes.push_back(classDesc);

        return m_classes.back();
    }

    template <typename T> struct TypeToEnum;
    template <> struct TypeToEnum<bool>         { static constexpr auto value = IPropertyDescriptor::Type::Bool; };
    template <> struct TypeToEnum<core::float4> { static constexpr auto value = IPropertyDescriptor::Type::Float4; };

    //--------------------------------------------------------------------------------------
    template <typename T> void ObjectFactory::ClassDesc::registerClassMemberT(const char * _propertyName, T * _offset, const char * _displayName)
    {
        VG_DEBUGPRINT("offsetof(%s, %s) = %u\n", name, _propertyName, _offset);

        #pragma warning(push)
        #pragma warning (disable: 4302)
        #pragma warning (disable: 4311)
        const u32 offset = (u32)(_offset);
        #pragma warning( pop )
    
        ClassProperty prop(_propertyName, TypeToEnum<T>::value, offset, _displayName);
    
        properties.push_back(prop);        
    }

    //--------------------------------------------------------------------------------------
    const IObjectDescriptor * ObjectFactory::getClassDescriptor(const char * _className) const
    {
        for (uint i = 0; i < m_classes.size(); ++i)
        {
            const auto & classDesc = m_classes[i];
            if (!strcmp(classDesc.name, _className))
                return &classDesc;
        }
        return nullptr;
    }
}