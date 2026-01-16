#pragma once

#include "core/IInstance.h"

namespace vg::core
{
    class Model;

    class Instance : public IInstance
    {
    public:
        VG_CLASS_DECL(Instance, IInstance);

                                        Instance                (const string & _name, IObject * _parent);
                                        ~Instance               ();

        void                            SetPropertyValue        (const IProperty & _prop, void * _previousValue, void * _newValue) override;
        void                            OnPropertyChanged       (IObject * _object, const IProperty & _prop, bool _notifyParent) override;

        void                            SetLocalMatrix          (const float4x4 & _local) override;
        const float4x4 &                GetLocalMatrix          () const final override;

        void                            OnLocalMatrixChanged    (bool _recomputeParents = false, bool _recomputeChildren = false) override;

        void                            SetGlobalMatrix         (const float4x4 & _global) override;
        const float4x4                  GetGlobalMatrix         () const final override;

        void                            SetColor                (const float4 & _color) final override;
        const float4 &                  GetColor                () const final override;

        void                            SetModel                (Lod _lod, IModel * _model) override;
        IModel *                        GetModel                (Lod _lod) const override;

        InstanceFlags                   GetInstanceFlags        () const final override;
        void                            SetInstanceFlags        (InstanceFlags _flags, bool _enabled) override;

        InstanceRuntimeFlags            GetInstanceRuntimeFlags () const override;
        void                            SetInstanceRuntimeFlags (InstanceRuntimeFlags _flags, bool _enabled) override;
        
        void                            Enable                  (bool _enable) override;

    public:
        VG_INLINE void                  setLocalMatrix          (const float4x4 & _local);
        VG_INLINE const float4x4 &      getLocalMatrix          () const;

        VG_INLINE void                  setGlobalMatrix         (const float4x4 & _global);
        VG_INLINE const float4x4        getGlobalMatrix         () const;

        VG_INLINE void                  setColor                (const float4 & _color);
        VG_INLINE const float4 &        getColor                () const;

        void                            setModel                (Lod _lod, Model * _model);
        VG_INLINE Model *               getModel                (Lod _lod) const;
        VG_INLINE bool                  hasAnyModel             () const;

        VG_INLINE InstanceFlags         getInstanceFlags        () const;
        VG_INLINE void                  setInstanceFlags        (InstanceFlags _flags, bool _enabled);

        VG_INLINE InstanceRuntimeFlags  getInstanceRuntimeFlags () const;
        VG_INLINE void                  setInstanceRuntimeFlags (InstanceRuntimeFlags _flags, bool _enabled);

        // Instance is enabled but it does not mean its parent are enabled
        VG_INLINE bool                  isLocalEnabled          () const;

        // Not only the instance is enable, but all its parents are enabled
        VG_INLINE bool                  isEnabledInHierarchy    () const;

        VG_INLINE bool                  isStatic                () const;
      
    private:
        InstanceFlags                   m_flags;   
        InstanceRuntimeFlags            m_runtimeFlags;
        float4                          m_color = core::float4(1.0f, 1.0f, 1.0f, 1.0f);
        float4x4                        m_local = float4x4::identity();
        float4x4                        m_global = float4x4::identity();
        vector<Model *>                 m_models;
    };
}

#if VG_ENABLE_INLINE
#include "Instance.inl"
#endif