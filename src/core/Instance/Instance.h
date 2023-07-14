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

        void                        SetWorldMatrix      (const float4x4 & _world) final;
        const float4x4 &            GetWorldMatrix      () const final override;

        void                        SetColor            (const float4 & _color) final;
        const float4 &              GetColor            () const final;

        void                        SetModel            (Lod _lod, IModel * _model) final;
        IModel *                    GetModel            (Lod _lod) const final;

        Flags                       GetFlags            () const final;
        void                        SetFlags            (Flags flags, bool enabled) final;

    public:
        VG_INLINE void              setWorldMatrix      (const float4x4 & _world);
        VG_INLINE const float4x4 &  getWorldMatrix      () const;

        VG_INLINE void              setColor            (const float4 & _color);
        VG_INLINE const float4 &    getColor            () const;

        void                        setModel            (Lod _lod, Model * _model);
        VG_INLINE Model *           getModel            (Lod _lod) const;

        VG_INLINE Flags             getFlags            () const;
        VG_INLINE void              setFlags            (Flags flags, bool enabled);
      
    private:
        Flags                       m_flags;
        float4                      m_color = { 1.0f, 1.0f, 1.0f, 1.0f };
        float4x4                    m_world = float4x4::identity();
        vector<Model *>             m_models;
    };
}

#if VG_ENABLE_INLINE
#include "Instance.inl"
#endif