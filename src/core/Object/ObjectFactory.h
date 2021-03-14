#pragma once

#include "core/IObjectFactory.h"

namespace vg::core
{
    class Object;

    class ObjectFactory : public IObjectFactory
    {
    public:
        class ClassProperty : public IPropertyDescriptor
        {
        public:
            ClassProperty(const char * _name, Type _type, uint_ptr _offset, const char * _prettyName, Flags _flags) :
                name(_name),
                type(_type),
                offset(_offset),
                displayName(_prettyName),
                flags(_flags)
            {

            }

            const char *    getName         () const final;
            Type            getType         () const final;
            uint_ptr        getOffset       () const final;
            const char *    getDisplayName  () const final;
            Flags           getFlags        () const final;           

        private:
            const char * name           = nullptr;
            const char * displayName    = nullptr;
            Type type                   = Type::Undefined;
            uint_ptr offset             = (uint_ptr)-1;
            Flags flags                 = Flags::None;
        };

        struct ClassDesc : public IObjectDescriptor
        {
            void                        registerProperty    (const char * _propertyName, bool * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags) final;
            void                        registerProperty    (const char * _propertyName, core::float4 * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags) final;
            void                        registerProperty    (const char * _propertyName, core::string * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags) final;
            void                        registerProperty    (const char * _propertyName, IPropertyDescriptor::Func _funcPtr, const char * _displayName, IPropertyDescriptor::Flags _flags) final;

            const char *                getClassName        () const final;
            uint                        getPropertyCount    () const final;
            const IPropertyDescriptor * getProperty         (uint _index) const final;

            const char *                name = nullptr;
            CreateFunc                  createFunc;
            vector<ClassProperty>       properties;

        private:
            template <typename T> void  registerClassMemberT(const char * _propertyName, T * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags);
        };

        IObjectDescriptor &             registerClass       (const char * _className, CreateFunc _createFunc) final;
        const IObjectDescriptor *       getClassDescriptor  (const char * _className) const final;
        bool                            isRegisteredClass   (const char * _className) const final;

        bool                            serializeFromString (IObject * _object, const string & _in) const final;
        bool                            serializeToString   (string & _out, const IObject * _object) const final;

    private:
        core::vector<ClassDesc> m_classes;
    };
}