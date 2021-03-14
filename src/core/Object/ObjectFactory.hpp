#include "ObjectFactory.h"
#include "core/Math/Math.h"

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
    IPropertyDescriptor::Flags ObjectFactory::ClassProperty::getFlags() const
    {
        return flags;
    }

    //--------------------------------------------------------------------------------------
    uint_ptr ObjectFactory::ClassProperty::getOffset() const
    {
        return offset;
    }

    //--------------------------------------------------------------------------------------
    void ObjectFactory::ClassDesc::registerProperty(const char * _propertyName, bool * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags)
    {
        registerClassMemberT(_propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ObjectFactory::ClassDesc::registerProperty(const char * _propertyName, core::float4 * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags)
    {
        registerClassMemberT(_propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ObjectFactory::ClassDesc::registerProperty(const char * _propertyName, core::string * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags)
    {
        registerClassMemberT(_propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ObjectFactory::ClassDesc::registerProperty(const char * _propertyName, core::IResource ** _offset, const char * _displayName, IPropertyDescriptor::Flags _flags)
    {
        registerClassMemberT(_propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ObjectFactory::ClassDesc::registerProperty(const char * _propertyName, core::IObject ** _offset, const char * _displayName, IPropertyDescriptor::Flags _flags)
    {
        registerClassMemberT(_propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ObjectFactory::ClassDesc::registerProperty(const char * _propertyName, IPropertyDescriptor::Func _funcPtr, const char * _displayName, IPropertyDescriptor::Flags _flags)
    {
        ClassProperty prop(_propertyName, IPropertyDescriptor::Type::Function, (uint_ptr)_funcPtr, _displayName, _flags);

        properties.push_back(prop);
    }

    //--------------------------------------------------------------------------------------
    const char * ObjectFactory::ClassDesc::getClassName() const
    {
        return name;
    }

    //--------------------------------------------------------------------------------------
    const char * ObjectFactory::ClassDesc::getClassDisplayName() const
    {
        return displayName;
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
    IObjectDescriptor::Func ObjectFactory::ClassDesc::getCreateFunc() const
    {
        return createFunc;
    }

    //--------------------------------------------------------------------------------------
    IObjectDescriptor::Flags ObjectFactory::ClassDesc::getFlags() const
    {
        return flags;
    }

    //--------------------------------------------------------------------------------------
    // Returned pointer shall not be stored but used immediatly ;)
    //--------------------------------------------------------------------------------------
    IObjectDescriptor & ObjectFactory::registerClass(const char * _className, const char * _displayName, IObjectDescriptor::Flags _flags, IObjectDescriptor::Func _createFunc)
    {
        VG_ASSERT(!isRegisteredClass(_className), "Class \"%s\" is already registered", _className);

        ClassDesc classDesc;
                  classDesc.name = _className;
                  classDesc.displayName = _displayName ? _displayName : _className;
                  classDesc.flags = _flags;
                  classDesc.createFunc = _createFunc;

        m_classes.push_back(classDesc);

        return m_classes.back();
    }

    //--------------------------------------------------------------------------------------
    bool ObjectFactory::isRegisteredClass(const char * _className) const
    {
        for (uint i = 0; i < m_classes.size(); ++i)
        {
            const auto & classDesc = m_classes[i];
            if (!strcmp(classDesc.name, _className))
                return true;
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    Object * ObjectFactory::getSingleton(const char * _className) const
    {
        const auto * desc = getClassDescriptor(_className);
        if (desc)
        {
            VG_ASSERT(asBool(IObjectDescriptor::Flags::Singleton & desc->getFlags()));
            return desc->getCreateFunc()();
        }
        return nullptr;
    }

    template <typename T> struct TypeToEnum;
    template <> struct TypeToEnum<bool>                         { static constexpr auto value = IPropertyDescriptor::Type::Bool; };
    template <> struct TypeToEnum<core::float4>                 { static constexpr auto value = IPropertyDescriptor::Type::Float4; };
    template <> struct TypeToEnum<core::string>                 { static constexpr auto value = IPropertyDescriptor::Type::String; };
    template <> struct TypeToEnum<IObject*>                     { static constexpr auto value = IPropertyDescriptor::Type::Object; };
    template <> struct TypeToEnum<IResource*>                   { static constexpr auto value = IPropertyDescriptor::Type::Resource; };
    template <> struct TypeToEnum<IPropertyDescriptor::Func>    { static constexpr auto value = IPropertyDescriptor::Type::Function; };

    //--------------------------------------------------------------------------------------
    template <typename T> void ObjectFactory::ClassDesc::registerClassMemberT(const char * _propertyName, T * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags)
    {
        #pragma warning(push)
        #pragma warning (disable: 4302)
        #pragma warning (disable: 4311)
        const u32 offset = (u32)(_offset);
        #pragma warning( pop )
    
        ClassProperty prop(_propertyName, TypeToEnum<T>::value, offset, _displayName, _flags);
    
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

        VG_ASSERT(false, "class \"%s\" has no class descriptor", _className);
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    bool ObjectFactory::serializeFromString(IObject * _object, const string & _in) const
    {
        string className;
        ClassDesc * desc = nullptr;
        map<string, string> values;

        if (!_in.empty())
        {
            size_t offset = 0;

            // get class name
            auto beginClassName = _in.find("[", offset);
            if (string::npos != beginClassName)
            {
                auto endClassName = _in.find("]", offset);
                if (string::npos != endClassName)
                {
                    className = _in.substr(beginClassName + 1, endClassName - 1);
                    offset = endClassName + 1;
                }
            }

            desc = (ClassDesc*)getClassDescriptor(className.c_str());
            if (nullptr == desc)
            {
                VG_ASSERT(false, "Cannot serialize unknown class \"%s\"", className.c_str());
                return false;
            }

            while (true)
            {
                auto keyOffset = _in.find_first_not_of("\n", offset);

                if (string::npos != keyOffset)
                {
                    auto keyEndOffset = _in.find("=", offset);
                    string key = _in.substr(keyOffset, keyEndOffset - keyOffset);

                    auto beginOffset = _in.find("=", keyOffset);
                    auto endOffset = _in.find("\n", beginOffset + 2);

                    if (string::npos != beginOffset && string::npos != endOffset)
                    {
                        string value = _in.substr(beginOffset + 1, endOffset - (beginOffset + 1));
                        values.insert({ key, value });

                        offset = endOffset + 1;
                    }
                }
                else
                    break;
            }
        }

        for (uint p = 0; p < desc->getPropertyCount(); ++p)
        {
            const auto & prop = desc->getProperty(p);
            const auto * name = prop->getName();
            const auto type = prop->getType();
            const auto offset = prop->getOffset();

            auto it = values.find(name);
            if (values.end() != it)
            {
                switch (type)
                {
                    default:
                        VG_ASSERT_ENUM_NOT_IMPLEMENTED(type);
                        break;

                    case IPropertyDescriptor::Type::Function:
                        break;

                    case IPropertyDescriptor::Type::Bool:
                    {
                        bool * pBool = (bool*)(uint_ptr(_object) + offset);
                        *pBool = (!strcmp(it->second.c_str(), "true") || !strcmp(it->second.c_str(), "TRUE") || !strcmp(it->second.c_str(), "1")) ? true : false;
                    }
                    break;

                    case IPropertyDescriptor::Type::String:
                    {
                        string * pString = (string*)(uint_ptr(_object) + offset);
                        *pString = it->second.substr(1, it->second.length()-2);
                    }
                    break;

                    case IPropertyDescriptor::Type::Float4:
                    {
                        float4 * pFloat4 = (float4*)(uint_ptr(_object) + offset);
                        float f[4] = {0.0f, 0.0f, 0.0f, 0.0f};

                        auto beginFloat4 = _in.find("{", offset);
                        if (string::npos != beginFloat4)
                        {
                            auto cur = beginFloat4;

                            auto endFloat4 = _in.find("}", cur);
                            if (string::npos != endFloat4)
                            {
                                for (uint i = 0; i < 4; ++i)
                                {
                                    auto endFloat = min(_in.find(",", cur), endFloat4);
                                    const string val = _in.substr(cur+1, endFloat - cur-1);
                                    f[i] = stof(val);
                                    cur = endFloat+1;
                                }
                            }
                        }

                        *pFloat4 = float4(f[0], f[1], f[2], f[3]);
                    }
                    break;
                }
            }
        }

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool ObjectFactory::serializeToString(string & _out, const IObject * _object) const
    {
        const char * className = _object->getClassName();
        _out += (string)"[" + className + "]" + "\n";

        const auto * classDesc = getClassDescriptor(className);
        
        for (uint p = 0; p < classDesc->getPropertyCount(); ++p)
        {
            const auto & prop = classDesc->getProperty(p);

            const auto name = prop->getName();
            const auto type = prop->getType();
            const auto offset = prop->getOffset();

            string value;
        
            switch (type)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(type);
                    break;

                case IPropertyDescriptor::Type::Function:
                    continue;

                case IPropertyDescriptor::Type::Bool:
                    value = *(bool*)(uint_ptr(_object) + offset) ? "true" : "false";
                    break;

                case IPropertyDescriptor::Type::String:
                    value = "\"" + *(string*)(uint_ptr(_object) + offset) + "\"";
                    break;

                case IPropertyDescriptor::Type::Float4:
                {
                    float4 * v = (float4*)(uint_ptr(_object) + offset);
                    value = "{" + to_string(v->x) + ", " + to_string(v->y) + ", " + to_string(v->z) + ", " + to_string(v->w) + "}";
                }
                break;
            }

            _out += (string)name + "=" + value + "\n";
        }

        return true;
    }
}