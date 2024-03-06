#pragma once

#include "core/Component/Component.h"
#include "engine/Resource/Mesh/MeshResource.h"
#include "MaterialResourceList.h"
#include "core/Misc/BitMask/BitMask.h"

namespace vg::renderer
{
    class IMeshInstance;
}

namespace vg::engine
{
    class AnimationJob;
    class DrawSkeletonJob;

    class MeshComponent : public core::Component
    {
    public:
        VG_CLASS_DECL(MeshComponent, core::Component);

                                            MeshComponent       (const core::string & _name, IObject * _parent);
                                            ~MeshComponent      ();

        void                                Update              (float _dt) override;

        void                                OnPropertyChanged   (IObject * _object, const core::IProperty & _prop, bool _notifyParent) override;
        void                                SetComponentFlags   (core::ComponentFlags _flags, bool _enabled) final override;

        const core::string                  GetSubObjectName    (core::uint _subObjectIndex) const override;
        
        void                                onResourceLoaded    (core::IResource * _resource) override;
        void                                onResourceUnloaded  (core::IResource * _resource) override;

        bool                                TryGetAABB          (core::AABB & _aabb) const final override;

        VG_INLINE MeshResource &            getMeshResource     ();
        VG_INLINE renderer::IMeshInstance * getMeshInstance     () const;

    private:
        renderer::IMeshInstance *           m_meshInstance      = nullptr;
        AnimationJob *                      m_updateSkeletonJob = nullptr;
        DrawSkeletonJob *                   m_drawSkeletonJob   = nullptr;
        MeshResource                        m_meshResource;
        MaterialResourceList                m_meshMaterials;
        core::BitMask                       m_batchMask;
        bool                                m_displayBones      = false;
        bool                                m_registered        = false;
    };
}

#if VG_ENABLE_INLINE
#include "MeshComponent.inl"
#endif