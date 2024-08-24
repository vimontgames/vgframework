#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class IModel;
    class AABB;

    enum class Lod : core::u8
    {
        Lod0 = 0,
        Lod1,
        Lod2,
        Lod3
    };

    enum class InstanceFlags : u32
    {
        Enabled         = 0x00000001,   // Instance is enabled and its enabled components are active
        Static          = 0x00000002,   // Instance doesn't move or change
    };

    class IInstance : public Object
    {
    public:
        IInstance(const string & _name, IObject * _parent) :
            Object(_name, _parent)
        {

        }

        virtual void                SetLocalMatrix      (const float4x4 & _local) = 0;
        virtual const float4x4 &    GetLocalMatrix      () const = 0;

        virtual void                SetGlobalMatrix     (const float4x4 & _global) = 0;
        virtual const float4x4      GetGlobalMatrix     () const = 0;

        virtual void                SetColor            (const float4 & _color) = 0;
        virtual const float4 &      GetColor            () const = 0;

        virtual void                SetModel            (Lod _lod, IModel * _model) = 0;
        virtual IModel *            GetModel            (Lod _lod) const = 0;

        virtual InstanceFlags       GetInstanceFlags    () const = 0;
        virtual void                SetInstanceFlags    (InstanceFlags _flags, bool _enabled) = 0;

        virtual bool                TryGetAABB          (AABB & _aabb) const = 0;
    };
}