#pragma once

#include "core/Component/Component.h"

namespace vg::engine
{
    class IAttachToNodeComponent : public core::Component
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IAttachToNodeComponent, core::Component);

        virtual void SetUseTarget(bool _useTarget) = 0;
        virtual void SetTarget(core::IGameObject * _target) = 0;
        virtual void SetBoneName(const core::string & _boneName) = 0;
    };
}