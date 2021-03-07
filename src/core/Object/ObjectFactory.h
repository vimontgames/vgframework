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
            ClassProperty(const char * _name, Type _type, u32 _offset, const char * _prettyName) :
                name(_name),
                type(_type),
                offset(_offset),
                displayName(_prettyName)
            {

            }

            const char *    getName         () const override;
            const char *    getDisplayName  () const override;
            Type            getType         () const override;
            uint_ptr        getOffset       () const override;

        private:
            const char * name           = nullptr;
            const char * displayName    = nullptr;
            Type type                   = Type::Undefined;
            u32 offset                  = (u32)-1;
        };

        struct ClassDesc : public IObjectDescriptor
        {
            void                        registerProperty    (const char * _propertyName, bool * _offset, const char * _displayName) override;
            void                        registerProperty    (const char * _propertyName, core::float4 * _offset, const char * _displayName) override;

            const char *                getClassName        () const override;
            uint                        getPropertyCount    () const override;
            const IPropertyDescriptor * getProperty         (uint _index) const override;

            const char *                name = nullptr;
            CreateFunc                  createFunc;
            vector<ClassProperty>       properties;

        private:
            template <typename T> void  registerClassMemberT(const char * _propertyName, T * _offset, const char * _displayName);
        };

        IObjectDescriptor &             registerClass       (const char * _className, CreateFunc _createFunc) override;
        const IObjectDescriptor *       getClassDescriptor  (const char * _className) const override;

    private:
        core::vector<ClassDesc> m_classes;
    };
}