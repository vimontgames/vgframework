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

        void                        SetColor            (const float4 & _color) final override;
        const float4 &              GetColor            () const final override;

        void                        SetModel            (Lod _lod, IModel * _model) override;
        IModel *                    GetModel            (Lod _lod) const override;

        Flags                       GetFlags            () const final override;
        void                        SetFlags            (Flags _flags, bool _enabled) final override;

        RuntimeFlags                GetRuntimeFlags     () const final override;
        void                        SetRuntimeFlags     (RuntimeFlags _flags, bool _enabled) final override;

    public:
        VG_INLINE void              setWorldMatrix      (const float4x4 & _world);
        VG_INLINE const float4x4 &  getWorldMatrix      () const;

        VG_INLINE void              setColor            (const float4 & _color);
        VG_INLINE const float4 &    getColor            () const;

        void                        setModel            (Lod _lod, Model * _model);
        VG_INLINE Model *           getModel            (Lod _lod) const;

        VG_INLINE Flags             getFlags            () const;
        VG_INLINE void              setFlags            (Flags _flags, bool _enabled);

        VG_INLINE RuntimeFlags      getRuntimeFlags     () const;
        VG_INLINE void              setRuntimeFlags     (RuntimeFlags _flags, bool _enabled);
      
    private:
        Flags                       m_flags;
        RuntimeFlags                m_runtimeFlags;
        float4                      m_color = { 1.0f, 1.0f, 1.0f, 1.0f };
        float4x4                    m_world = float4x4::identity();
        vector<Model *>             m_models;
    };
}

#if VG_ENABLE_INLINE
#include "Instance.inl"
#endif