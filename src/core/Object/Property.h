#pragma once

#include "core/IClassDesc.h"

namespace vg::core
{
    struct EnumDesc 
    {
        string name;
        uint value = 0x0;
    };

    class Property : public IProperty
    {
    public:
                                Property        (const char * _name, Type _type, uint_ptr _offset, core::u32 _value, const char * _prettyName, Flags _flags, uint _enumCount = 0, const char * _enumNames = nullptr, const void * _enumValues = nullptr);
                                Property        (const Property & other);
                                ~Property       ();

        void                    setRange        (float2 _range) final;

        const char *            getName         () const final;
        Type                    getType         () const final;
        uint_ptr                getOffset       () const final;
		core::u32		        getValue		() const final;
        const char *            getDisplayName  () const final;
        Flags                   getFlags        () const final; 
        float2                  getRange        () const final;
        u32                     getEnumCount    () const final;
        const char *            getEnumName     (uint index) const final;
        u32                     getEnumValue    (uint index) const final;

    protected:
        template <typename T> void   initEnum   (uint _enumCount, const char * _enumNames, const void * _enumValues);

    private:
        const char *            name            = nullptr;
        const char *            displayName     = nullptr;
        Type		            type			= Type::Undefined;
        uint_ptr	            offset			= (uint_ptr)-1;
		core::u32	            value			= 0x0;
        Flags		            flags			= Flags::None;
        float2		            range			= float2(0.0f, 0.0f);
        vector<EnumDesc>        enums;
    };   
}