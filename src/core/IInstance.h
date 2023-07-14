#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class IModel;

    enum class Lod
    {
        Lod0 = 0,
        Lod1,
        Lod2,
        Lod3
    };

    class IInstance : public Object
    {
    public:
        enum Flags : u32
        {
            Enabled = 0x00000001,
            Selected = 0x00000002
        };

        IInstance(const string & _name, IObject * _parent) :
            Object(_name, _parent)
        {

        }

        virtual void                SetWorldMatrix  (const float4x4 & _world) = 0;
        virtual const float4x4 &    GetWorldMatrix  () const = 0;

        virtual void                SetColor        (const float4 & _color) = 0;
        virtual const float4 &      GetColor        () const = 0;

        virtual void                SetModel        (Lod _lod, IModel * _model) = 0;
        virtual IModel *            GetModel        (Lod _lod) const = 0;

        virtual Flags               GetFlags        () const = 0;
        virtual void                SetFlags        (Flags flags, bool enabled) = 0;
    };
}