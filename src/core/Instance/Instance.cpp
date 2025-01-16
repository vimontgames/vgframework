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
        m_flags(InstanceFlags::Enabled)
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
        setPropertyDescription(Instance, m_flags, "Instance flags");

        registerPropertyEx(Instance, m_color, "Color", PropertyFlags::Color);
        setPropertyDescription(Instance, m_color, "Instance color");

        registerPropertyEx(Instance, m_local, "Transform", PropertyFlags::Flatten);
        setPropertyDescription(Instance, m_local, "Local transform");

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
    void Instance::SetPropertyValue(const IProperty & _prop, void * _previousValue, void * _newValue)
    {
        if (&m_flags == _previousValue)
        {
            auto newFlags = *(InstanceFlags *)_newValue;

            if ((InstanceFlags::Enabled & m_flags) != (InstanceFlags::Enabled & newFlags))
            {
                if (asBool(InstanceFlags::Enabled & newFlags))
                    OnEnable();
                else
                    OnDisable();
            }
        }

        super::SetPropertyValue(_prop, _previousValue, _newValue);
    }

    //--------------------------------------------------------------------------------------
    void Instance::SetInstanceFlags(InstanceFlags _flags, bool _enabled)
    {
        setInstanceFlags(_flags, _enabled);

        if (asBool(_flags & InstanceFlags::Enabled))
        {
            if (_enabled)
                OnEnable();
            else
                OnDisable();
        }
    }

    //--------------------------------------------------------------------------------------
    void Instance::Enable(bool _enable)
    {
        SetInstanceFlags(InstanceFlags::Enabled, _enable);
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