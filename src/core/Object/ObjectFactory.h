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
            ClassProperty(const char * _name, Type _type, uint_ptr _offset, core::u32 _value, const char * _prettyName, Flags _flags, const char * _description = nullptr) :
                name(_name),
                type(_type),
                offset(_offset),
				value(_value),
                displayName(_prettyName),
                flags(_flags),
                description(_description)
            {

            }

            void            setRange        (float2 _range) final;

            const char *    getName         () const final;
            Type            getType         () const final;
            uint_ptr        getOffset       () const final;
			core::u32		getValue		() const final;
            const char *    getDisplayName  () const final;
            const char *    getDescription  () const final;
            Flags           getFlags        () const final; 
            float2          getRange        () const final;

        private:
            const char *    name            = nullptr;
            const char *    displayName     = nullptr;
            const char *    description     = nullptr;
            Type		    type			= Type::Undefined;
            uint_ptr	    offset			= (uint_ptr)-1;
			core::u32	    value			= 0x0;
            Flags		    flags			= Flags::None;
            float2		    range			= float2(0.0f, 0.0f);
        };

        struct ClassDesc : public IObjectDescriptor
        {
            bool                                isRegisteredProperty    (const char * _propertyName) final;

            void                                registerProperty        (const char * _propertyName, bool * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags) final;
            void                                registerProperty        (const char * _propertyName, core::u32 * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags) final;
            void                                registerProperty        (const char * _propertyName, core::u16 * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags) final;
            void                                registerProperty        (const char * _propertyName, float * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags) final;
            void                                registerProperty        (const char * _propertyName, float4 * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags) final;
            void                                registerProperty        (const char * _propertyName, float4x4 * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags) final;
            void                                registerProperty        (const char * _propertyName, string * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags) final;
            void                                registerProperty        (const char * _propertyName, IResource ** _offset, const char * _displayName, IPropertyDescriptor::Flags _flags) final;
            void                                registerProperty        (const char * _propertyName, IObject ** _offset, const char * _displayName, IPropertyDescriptor::Flags _flags) final;
            void                                registerProperty        (const char * _propertyName, IPropertyDescriptor::Func _funcPtr, const char * _displayName, IPropertyDescriptor::Flags _flags) final;
            void                                registerProperty        (const char * _propertyName, core::u32 _sizeOf, void * _offset, const char * _displayName = nullptr, IPropertyDescriptor::Flags _flags = IPropertyDescriptor::Flags::None) final;
            void                                registerProperty        (const char * _propertyName, vector<IObject*>* _offset, const char * _displayName = nullptr, IPropertyDescriptor::Flags _flags = IPropertyDescriptor::Flags::None) final;
            void                                registerProperty        (const char * _propertyName, dictionary<core::IObject*>* _offset, const char * _displayName = nullptr, IPropertyDescriptor::Flags _flags = IPropertyDescriptor::Flags::None) final;
			void								registerProperty		(const char * _propertyName, core::u32 * _offset, core::u32 _value, const char * _displayName, IPropertyDescriptor::Flags _flags) final;
            void								registerEnum            (const char * _propertyName, core::u32 * _offset, core::u32 _value, const char * _displayName, const char * _values, IPropertyDescriptor::Flags _flags) final;

            const char *                        getClassName            () const final;
            const char *                        getClassDisplayName     () const final;
            uint                                getPropertyCount        () const final;
            const IPropertyDescriptor *         getPropertyByIndex      (uint _index) const final;
            IPropertyDescriptor *               getPropertyByName       (const char * _propertyName) const final;
            IObjectDescriptor::Func             getCreateFunc           () const final;
            IObjectDescriptor::SingletonFunc    getSingletonFunc        () const final;
            IObjectDescriptor::Flags            getFlags                () const final;
            u32                                 getNextIndex            () const final;

            const char *                        name = nullptr;
            const char *                        displayName = nullptr;
            IObjectDescriptor::Flags            flags = IObjectDescriptor::Flags::None;
            IObjectDescriptor::Func             createFunc;
            IObjectDescriptor::SingletonFunc    createSingletonFunc;
            vector<ClassProperty>               properties;
            mutable u32                         count = 0;

        private:
            template <typename T> void  registerClassMemberT    (const char * _propertyName, T * _offset, core::u32 _value, const char * _displayName, IPropertyDescriptor::Flags _flags);
        };

        IObjectDescriptor *             registerClass           (const char * _className, const char * _displayName, IObjectDescriptor::Flags _flags, IObjectDescriptor::Func _createFunc) final;
        IObjectDescriptor *             registerSingletonClass  (const char * _className, const char * _displayName, IObjectDescriptor::Flags _flags, IObjectDescriptor::SingletonFunc _createFunc) final;
        const IObjectDescriptor *       getClassDescriptor      (const char * _className) const final;
        bool                            isRegisteredClass       (const char * _className) const final;
        IObject *                       getSingleton            (const char * _className) const final;
        IObject *                       createObject            (const char * _className, const string & _name, IObject * _parent = nullptr) const final;

        bool                            serializeFromString     (IObject * _object, const string & _in) const final;
        bool                            serializeToString       (string & _out, const IObject * _object) const final;

    private:
        core::vector<ClassDesc>         m_classes;
    };    
}