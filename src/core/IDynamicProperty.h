#pragma once

#include "core/GameObject/GameObject.h"

namespace vg::core
{
    class IDynamicProperty : public Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IDynamicProperty, Object);

        virtual IProperty * GetProperty() const = 0;
        virtual void Enable(bool _enable) = 0;
        virtual bool IsEnable() const = 0;
        virtual bool BackupOriginalValue(const IObject * _object, const IProperty * _prop) = 0;
        virtual bool RestoreOriginalValue(IObject * _object, const IProperty * _prop) = 0;
        virtual bool ApplyOverride(IObject * _object, const IProperty * _prop) = 0;
        virtual bool SetOverrideInitValue(const IObject * _object, const IProperty * _prop) = 0;        
    };
}