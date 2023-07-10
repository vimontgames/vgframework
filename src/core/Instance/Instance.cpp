#include "core/Precomp.h"
#include "Instance.h"
#include "core/IFactory.h"
#include "core//Model/Model.h"
#include "core/Object/EnumHelper.h"

#if !VG_ENABLE_INLINE
#include "Instance.inl"
#endif

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    Instance::Instance(const string & _name, IObject * _parent) :
        IInstance(_name, _parent),
        m_flags(Flags::Enabled)
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

        EnumHelper<Flags> flagsEnum;
        _desc.registerPropertyEnumHelper(Instance, Flags, m_flags, "Flags", flagsEnum.getCount(), flagsEnum.getNames(), flagsEnum.getValues(), IProperty::Flags::Bitfield);
        _desc.registerPropertyHelper(Instance, m_color, "Color", IProperty::Flags::Color);
        _desc.registerPropertyHelper(Instance, m_world, "Matrix", IProperty::Flags::None);
        _desc.registerPropertyObjectRefVectorHelper(Instance, m_models, "Models", IProperty::Flags::ReadOnly | IProperty::Flags::NotSaved);

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
    IInstance::Flags Instance::GetFlags() const
    {
        return getFlags();
    }

    //--------------------------------------------------------------------------------------
    void Instance::SetFlags(Flags flags, bool enabled)
    {
        setFlags(flags, enabled);
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
            VG_SAFE_RELEASE(model);
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