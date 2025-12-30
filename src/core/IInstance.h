#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class IModel;
    class AABB;

    vg_enum_class(vg::core, Lod, u8,
        Lod0 = 0,
        Lod1,
        Lod2,
        Lod3
    );

    vg_enum_class(vg::core, InstanceFlags, u16,
        Enabled             = 0x0001,   // Instance is enabled and its enabled components are active if their parent is enabled
        Static              = 0x0002    // Instance doesn't move or change
    );

    vg_enum_class(vg::core, InstanceRuntimeFlags, u16,
        Active             = 0x0001   // Instance is actually enabled: it's enabled and all its parents are enabled
    );

    class IInstance : public Object
    {
    public:
        IInstance(const string & _name, IObject * _parent) :
            Object(_name, _parent)
        {

        }

        virtual void                    SetLocalMatrix          (const float4x4 & _local) = 0;
        virtual const float4x4 &        GetLocalMatrix          () const = 0;

        virtual void                    SetGlobalMatrix         (const float4x4 & _global) = 0;
        virtual const float4x4          GetGlobalMatrix         () const = 0;

        virtual void                    OnLocalMatrixChanged    (bool _recomputeParents = false, bool _recomputeChildren = false) = 0;

        virtual void                    SetColor                (const float4 & _color) = 0;
        virtual const float4 &          GetColor                () const = 0;

        virtual void                    SetModel                (Lod _lod, IModel * _model) = 0;
        virtual IModel *                GetModel                (Lod _lod) const = 0;

        virtual InstanceFlags           GetInstanceFlags        () const = 0;
        virtual void                    SetInstanceFlags        (InstanceFlags _flags, bool _enabled) = 0;

        virtual InstanceRuntimeFlags    GetInstanceRuntimeFlags () const = 0;
        virtual void                    SetInstanceRuntimeFlags (InstanceRuntimeFlags _flags, bool _enabled) = 0;

        virtual void                    Enable                  (bool _enable) = 0;

        virtual bool                    TryGetAABB              (AABB & _aabb) const = 0;
    };
}