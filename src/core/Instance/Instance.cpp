#include "core/Precomp.h"
#include "Instance.h"
#include "core/IObjectFactory.h"
#include "core//Model/Model.h"

#if !VG_ENABLE_INLINE
#include "Instance.inl"
#endif

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    Instance::Instance(const string & _name, IObject * _parent) :
        IInstance(_name, _parent)
    {
        for (uint i = 0; i < enumCount<Lod>(); ++i)
            m_models[i] = nullptr;
    }

    //--------------------------------------------------------------------------------------
    Instance::~Instance()
    {
        for (uint i = 0; i < enumCount<Lod>(); ++i)
            VG_SAFE_RELEASE(m_models[i]);
    }

    //--------------------------------------------------------------------------------------
    bool Instance::registerProperties(IObjectDescriptor & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerPropertyHelper(Instance, m_world, "Matrix", IPropertyDescriptor::Flags::None);

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
    void Instance::SetModel(Lod _lod, IModel * _model)
    {
        VG_ASSERT(isEnumValue(_lod));

        auto & model = m_models[asInteger(_lod)];
        if (_model != model)
        {
            VG_SAFE_INCREASE_REFCOUNT(_model);
            VG_SAFE_RELEASE(model);
            model = (Model*)_model;
        }
    }

    //--------------------------------------------------------------------------------------
    IModel * Instance::GetModel(Lod _lod) const
    {
        return getModel(_lod);
    }
}