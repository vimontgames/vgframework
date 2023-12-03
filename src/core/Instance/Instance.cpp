#include "core/Precomp.h"
#include "Instance.h"
#include "core/IFactory.h"
#include "core//Model/Model.h"
#include "core/Object/EnumHelper.h"
#include "core/Kernel.h"

#if !VG_ENABLE_INLINE
#include "Instance.inl"
#endif

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    Instance::Instance(const string & _name, IObject * _parent) :
        IInstance(_name, _parent),
        m_flags(Flags::Enabled),
        m_runtimeFlags((RuntimeFlags)0x0)
    {
    }

    //--------------------------------------------------------------------------------------
    Instance::~Instance()
    {
        for (uint i = 0; i < m_models.size(); ++i)
            VG_SAFE_RELEASE(m_models[i]);
    }

    //--------------------------------------------------------------------------------------
    bool Instance::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerPropertyEnumBitfield(Instance, Flags, m_flags, "Flags");
        _desc.registerPropertyEx(Instance, m_color, "Color", IProperty::Flags::Color);
        _desc.registerProperty(Instance, m_world, "Matrix");
        _desc.registerPropertyObjectPtrVectorEx(Instance, m_models, "Models", IProperty::Flags::ReadOnly | IProperty::Flags::NotSaved);

        return true;
    }

    //--------------------------------------------------------------------------------------
    void Instance::SetWorldMatrix(const float4x4 & _world)
    {
        setWorldMatrix(_world);
    }

    //--------------------------------------------------------------------------------------
    const float4x4 & Instance::GetWorldMatrix() const
    {
        return getWorldMatrix();
    }

    //--------------------------------------------------------------------------------------
    void Instance::SetColor(const float4 & _color)
    {
        setColor(_color);
    }
    //--------------------------------------------------------------------------------------
    const float4 & Instance::GetColor() const
    {
        return getColor();
    }

    //--------------------------------------------------------------------------------------
    IInstance::Flags Instance::GetFlags() const
    {
        return getFlags();
    }

    //--------------------------------------------------------------------------------------
    void Instance::SetFlags(Flags flags, bool _enabled)
    {
        setFlags(flags, _enabled);
    }

    //--------------------------------------------------------------------------------------
    IInstance::RuntimeFlags Instance::GetRuntimeFlags() const
    {
        return getRuntimeFlags();
    }

    //--------------------------------------------------------------------------------------
    void Instance::SetRuntimeFlags(RuntimeFlags _flags, bool _enabled)
    {
        setRuntimeFlags(_flags, _enabled);
    }

    //--------------------------------------------------------------------------------------
    void Instance::setModel(Lod _lod, Model * _model)
    {
        VG_ASSERT(isEnumValue(_lod));

        if (asInteger(_lod) >= m_models.size())
            m_models.resize(asInteger(_lod) + 1);

        auto & model = m_models[asInteger(_lod)];
        if (_model != model)
        {
            VG_SAFE_INCREASE_REFCOUNT(_model);
            VG_SAFE_RELEASE_ASYNC(model);
            model = _model;
        }
    }

    //--------------------------------------------------------------------------------------
    void Instance::SetModel(Lod _lod, IModel * _model)
    {
        setModel(_lod, (Model*)_model);
    }

    //--------------------------------------------------------------------------------------
    IModel * Instance::GetModel(Lod _lod) const
    {
        return getModel(_lod);
    }
}