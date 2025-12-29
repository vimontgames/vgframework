#pragma once

#include "core/Resource/Resource.h"

namespace vg::engine
{
    using MatPropIndex = core::uint;

    class IMaterialResource : public core::Resource
    {
        public:
            VG_CLASS_DECL_ABSTRACT(IMaterialResource, core::Resource);

            virtual MatPropIndex    GetPropertyIndex    (const core::string & _propertyName) const = 0;
            virtual void            SetProperty         (MatPropIndex _propertyIndex, float _value) = 0;
    };
}