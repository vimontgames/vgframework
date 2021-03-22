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

            void            setRange        (float2 _range) final;

            const char *    getName         () const final;
            Type            getType         () const final;
            uint_ptr        getOffset       () const final;
            const char *    getDisplayName  () const final;
            Flags           getFlags        () const final; 
            float2          getRange        () const final;

        private:
            const char * name           = nullptr;
            const char * displayName    = nullptr;
            Type type                   = Type::Undefined;
            uint_ptr offset             = (uint_ptr)-1;
            Flags flags                 = Flags::None;
            float2 range                = float2(0.0f, 0.0f);
        };

        struct ClassDesc : public IObjectDescriptor
        {
            bool                        isRegisteredProperty    (const char * _propertyName) final;

            void                        registerProperty        (const char * _propertyName, bool * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags) final;
            void                        registerProperty        (const char * _propertyName, float * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags) final;
            void                        registerProperty        (const char * _propertyName, float4 * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags) final;
            void                        registerProperty        (const char * _propertyName, string * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags) final;
            void                        registerProperty        (const char * _propertyName, IResource ** _offset, const char * _displayName, IPropertyDescriptor::Flags _flags) final;
            void                        registerProperty        (const char * _propertyName, IObject ** _offset, const char * _displayName, IPropertyDescriptor::Flags _flags) final;
            void                        registerProperty        (const char * _propertyName, IPropertyDescriptor::Func _funcPtr, const char * _displayName, IPropertyDescriptor::Flags _flags) final;

            void                        registerProperty        (const char * _propertyName, vector<IObject*>* _offset, const char * _displayName = nullptr, IPropertyDescriptor::Flags _flags = IPropertyDescriptor::Flags::None) final;

            const char *                getClassName            () const final;
            const char *                getClassDisplayName     () const final;
            uint                        getPropertyCount        () const final;
            const IPropertyDescriptor * getPropertyByIndex      (uint _index) const final;
            IPropertyDescriptor *       getPropertyByName(const char * _propertyName) const final;
            IObjectDescriptor::Func     getCreateFunc           () const final;
            IObjectDescriptor::Flags    getFlags                () const final;

            const char *                name = nullptr;
            const char *                displayName = nullptr;
            IObjectDescriptor::Flags    flags = IObjectDescriptor::Flags::None;
            IObjectDescriptor::Func     createFunc;
            vector<ClassProperty>       properties;

        private:
            template <typename T> void  registerClassMemberT    (const char * _propertyName, T * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags);
        };

        IObjectDescriptor &             registerClass           (const char * _className, const char * _displayName, IObjectDescriptor::Flags _flags, IObjectDescriptor::Func _createFunc) final;
        const IObjectDescriptor *       getClassDescriptor      (const char * _className) const final;
        bool                            isRegisteredClass       (const char * _className) const final;
        Object *                        getSingleton            (const char * _className) const final;

        bool                            serializeFromString     (IObject * _object, const string & _in) const final;
        bool                            serializeToString       (string & _out, const IObject * _object) const final;

    private:
        core::vector<ClassDesc>         m_classes;
    };    
}