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

        void                        SetPropertyValue    (const IProperty & _prop, void * _previousValue, void * _newValue) override;

        void                        SetLocalMatrix      (const float4x4 & _local) final override;
        const float4x4 &            GetLocalMatrix      () const final override;

        void                        SetGlobalMatrix     (const float4x4 & _global) final override;
        const float4x4              GetGlobalMatrix     () const final override;

        void                        SetColor            (const float4 & _color) final override;
        const float4 &              GetColor            () const final override;

        void                        SetModel            (Lod _lod, IModel * _model) override;
        IModel *                    GetModel            (Lod _lod) const override;

        InstanceFlags               GetInstanceFlags    () const final override;
        void                        SetInstanceFlags    (InstanceFlags _flags, bool _enabled) override;

        RuntimeFlags                GetRuntimeFlags     () const final override;
        void                        SetRuntimeFlags     (RuntimeFlags _flags, bool _enabled) final override;

    public:
        VG_INLINE void              setLocalMatrix      (const float4x4 & _local);
        VG_INLINE const float4x4 &  getLocalMatrix      () const;

        VG_INLINE void              setGlobalMatrix     (const float4x4 & _global);
        VG_INLINE const float4x4    getGlobalMatrix     () const;

        VG_INLINE void              setColor            (const float4 & _color);
        VG_INLINE const float4 &    getColor            () const;

        void                        setModel            (Lod _lod, Model * _model);
        VG_INLINE Model *           getModel            (Lod _lod) const;

        VG_INLINE InstanceFlags     getInstanceFlags    () const;
        VG_INLINE void              setInstanceFlags    (InstanceFlags _flags, bool _enabled);

        VG_INLINE RuntimeFlags      getRuntimeFlags     () const;
        VG_INLINE void              setRuntimeFlags     (RuntimeFlags _flags, bool _enabled);

        VG_INLINE bool              isEnabled           () const;
        VG_INLINE bool              isStatic            () const;
      
    private:
        InstanceFlags               m_flags;
        RuntimeFlags                m_runtimeFlags;
        float4                      m_color = { 1.0f, 1.0f, 1.0f, 1.0f };
        float4x4                    m_local = float4x4::identity();
        vector<Model *>             m_models;
    };
}

#if VG_ENABLE_INLINE
#include "Instance.inl"
#endif