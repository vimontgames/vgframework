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

    class IInstance : public ObjectPointer
    {
    public:
        IInstance(const string & _name, IObject * _parent) :
            ObjectPointer(_name, _parent)
        {

        }

        virtual void SetWorldMatrix(const float4x4 & _world) = 0;
        virtual const float4x4 & GetWorldMatrix() const = 0;

        virtual void SetModel(Lod _lod, IModel * _model) = 0;
        virtual IModel * GetModel(Lod _lod) const = 0;
    };
}