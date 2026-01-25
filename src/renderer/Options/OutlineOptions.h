#pragma once

#include "core/Object/Object.h"
#include "core/Object/ObjectList.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class OutlineOptions : public core::Object
    {
    public:
        VG_CLASS_DECL(OutlineOptions, core::Object);

        OutlineOptions(const core::string & _name = "", core::IObject * _parent = nullptr);
        ~OutlineOptions();

        VG_INLINE void setZPassOutlineColor(const core::float4 & _color);
        VG_INLINE const core::float4 & getZPassOutlineColor() const;

        VG_INLINE void setZFailOutlineColor(const core::float4 & _color);
        VG_INLINE const core::float4 & getZFailOutlineColor() const;

    private:
        core::float4 m_zPassOutlineColor = core::float4(1.0f, 1.0f, 1.0f, 0.75f);
        core::float4 m_zFailOutlineColor = core::float4(1.0f, 1.0f, 1.0f, 0.25f);
    };

    //--------------------------------------------------------------------------------------
    class OutlineOptionsList : public core::ObjectList<OutlineOptions>
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(OutlineOptionsList, ObjectList<OutlineOptions>);
    };
}

#if VG_ENABLE_INLINE
#include "OutlineOptions.inl"
#endif