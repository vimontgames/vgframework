#pragma once

#include "engine/IMeshComponent.h"
#include "engine/Resource/Mesh/MeshResource.h"
#include "engine/Component/Renderer/MaterialResourceList.h"
#include "core/Misc/BitMask/BitMask.h"

namespace vg::renderer
{
    class IMeshInstance;
}

namespace vg::engine
{
    class AnimationJob;
    class DrawSkeletonJob;

    class MeshComponent : public IMeshComponent
    {
    public:
        VG_CLASS_DECL(MeshComponent, IMeshComponent);

                                            MeshComponent           (const core::string & _name, IObject * _parent);
                                            ~MeshComponent          ();

        void                                Update                  (const Context & _context) override;

        void                                OnPropertyChanged       (IObject * _object, const core::IProperty & _prop, bool _notifyParent) override;
        void                                EnableComponentFlags    (core::ComponentFlags _flags, bool _enabled) final override;

        const core::string                  GetSubObjectName        (core::uint _subObjectIndex) const override;
        
        void                                OnResourceLoaded        (core::IResource * _resource) override;
        void                                OnResourceUnloaded      (core::IResource * _resource) override;

        bool                                TryGetAABB              (core::AABB & _aabb) const final override;

        renderer::IGraphicInstance *        GetGraphicInstance      () final override;
        void                                RefreshGraphicInstance  () final override;

        // Materials
        MaterialIndex                       GetMaterialIndex        (const core::string & _materialName) const final override;
        IMaterialResource *                 GetMaterialResource     (MaterialIndex _materialIndex) const final override;

        bool                                isSkeletonVisible       () const;

        VG_INLINE const MeshResource &      getMeshResource         () const;
        VG_INLINE MeshResource &            getMeshResource         ();
        VG_INLINE renderer::IMeshInstance * getMeshInstance         () const;

    private:
        renderer::IMeshInstance *           m_meshInstance          = nullptr;
        AnimationJob *                      m_updateSkeletonJob     = nullptr;
        DrawSkeletonJob *                   m_drawSkeletonJob       = nullptr;
        MeshResource                        m_meshResource;
        MaterialResourceList                m_meshMaterials;
        core::BitMask                       m_batchMask;
    };
}

#if VG_ENABLE_INLINE
#include "MeshComponent.inl"
#endif