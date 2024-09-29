#pragma once

#include "engine/IAttachToNodeComponent.h"
#include "core/Object/ObjectHandle.h"

namespace vg::renderer
{
    class ISkeleton;
    using NodeIndex = core::i16;
}

namespace vg::engine
{
    class MeshComponent;

    class AttachToNodeComponent final : public IAttachToNodeComponent
    {
    public:
        VG_CLASS_DECL(AttachToNodeComponent, IAttachToNodeComponent);

        AttachToNodeComponent(const core::string & _name, IObject * _parent);
        ~AttachToNodeComponent();
    
        void                            OnLoad              () final override;
        void                            OnPlay              () final override;
        void                            OnStop              () final override;

        void                            Update              (const Context & _context) final override;
        void                            SetPropertyValue    (const core::IProperty & _prop, void * _previousValue, void * _newValue) final override;

        void                            SetUseTarget        (bool _useTarget) final override;
        void                            SetTarget           (core::IGameObject * _target) final override;
        void                            SetBoneName         (const core::string & _boneName) final override;

        bool                            updateCache         ();
        void                            clearCache          ();

    private:
        bool                            m_useTarget = false;
        core::ObjectHandle              m_target;
        core::string                    m_boneName;
        //bool                            m_useTransform = false;
        core::float4x4                  m_transform = core::float4x4::identity();

        const core::IGameObject *       m_gameObject = nullptr;
        const renderer::ISkeleton *     m_skeleton;
        renderer::NodeIndex             m_nodeIndex;
    };
}