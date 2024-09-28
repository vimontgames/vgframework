#pragma once

#include "core/Component/Component.h"
#include "core/Object/ObjectHandle.h"

namespace vg::renderer
{
    class ISkeleton;
    using NodeIndex = core::i16;
}

namespace vg::engine
{
    class MeshComponent;

    class AttachToNodeComponent final : public core::Component
    {
    public:
        VG_CLASS_DECL(AttachToNodeComponent, core::Component);

        AttachToNodeComponent(const core::string & _name, IObject * _parent);
        ~AttachToNodeComponent();
    
        void                            OnLoad              () final override;
        void                            OnEnable            () final override;
        void                            OnDisable           () final override;
        void                            Update              (const Context & _context) final override;
        void                            SetPropertyValue    (const core::IProperty & _prop, void * _previousValue, void * _newValue) final override;

        bool                            updateCache         ();
        void                            clearCache          ();

    private:
        bool                            m_useTarget = false;
        core::ObjectHandle              m_target;
        core::string                    m_boneName;
        const renderer::ISkeleton *     m_skeleton;
        renderer::NodeIndex             m_nodeIndex;
    };
}