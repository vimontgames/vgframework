#pragma once

#include "core/IInstance.h"

namespace vg::core
{
    class Model;

    class Instance : public IInstance
    {
        using super = IInstance;

    public:
                                    Instance            (const string & _name, IObject * _parent);
                                    ~Instance           ();

        static bool                 registerProperties  (IClassDesc & _desc);

        void                        SetWorldMatrix      (const float4x4 & _world) final override;
        const float4x4 &            GetWorldMatrix      () const final override;

        void                        SetModel            (Lod _lod, IModel * _model) final override;
        IModel *                    GetModel            (Lod _lod) const final override;

        VG_INLINE void              setWorldMatrix      (const float4x4 & _world);
        VG_INLINE const float4x4 &  getWorldMatrix      () const;

        void                        setModel            (Lod _lod, Model * _model);
        VG_INLINE Model *           getModel            (Lod _lod) const;
      
    private:
        float4x4                    m_world = float4x4::identity();
        vector<Model *>             m_models;
    };
}

#if VG_ENABLE_INLINE
#include "Instance.inl"
#endif