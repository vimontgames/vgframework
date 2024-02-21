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
        m_flags(InstanceFlags::Enabled),
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

        registerPropertyEnumBitfield(Instance, InstanceFlags, m_flags, "Flags");
        registerPropertyEx(Instance, m_color, "Color", IProperty::Flags::Color);
        registerProperty(Instance, m_local, "Transform");
        registerPropertyObjectPtrVectorEx(Instance, m_models, "Models", IProperty::Flags::ReadOnly | IProperty::Flags::NotSaved | IProperty::Flags::Hidden);

        return true;
    }

    //--------------------------------------------------------------------------------------
    void Instance::SetLocalMatrix(const float4x4 & _local)
    {
        setLocalMatrix(_local);
    }

    //--------------------------------------------------------------------------------------
    const float4x4 & Instance::GetLocalMatrix() const
    {
        return getLocalMatrix();
    }

    //--------------------------------------------------------------------------------------
    void Instance::SetGlobalMatrix(const float4x4 & _global)
    {
        setGlobalMatrix(_global);
    }

    //--------------------------------------------------------------------------------------
    const float4x4 Instance::GetGlobalMatrix() const
    {
        return getGlobalMatrix();
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
    InstanceFlags Instance::GetInstanceFlags() const
    {
        return getInstanceFlags();
    }

    //--------------------------------------------------------------------------------------
    void Instance::SetInstanceFlags(InstanceFlags flags, bool _enabled)
    {
        setInstanceFlags(flags, _enabled);
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