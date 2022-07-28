#pragma once

#include "core/IClassDesc.h"

namespace vg::core
{
    class Property : public IProperty
    {
    public:
        Property(const char * _name, Type _type, uint_ptr _offset, core::u32 _value, const char * _prettyName, Flags _flags, const char * _description = nullptr) :
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
}