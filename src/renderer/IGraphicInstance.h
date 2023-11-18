#pragma once

#include "core/Instance/Instance.h"

namespace vg::gfx
{
    class CommandList;
}

namespace vg::renderer
{
    class IMaterialModel;
    class RenderContext;
    using PickingID = core::uint;

    class IGraphicInstance : public core::Instance
    {
    public:
        IGraphicInstance(const core::string & _name, core::IObject * _parent) :
            core::Instance(_name, _parent)
        {
            resetPickingID();
        }

        virtual void                Draw        (const RenderContext & _renderContext, gfx::CommandList * _cmdList) const = 0;

        virtual bool                SetMaterial (core::uint _index, IMaterialModel * _materialModel) = 0;
        virtual IMaterialModel *    GetMaterial (core::uint _index) const = 0;

        void resetPickingID()
        {
            m_pickingID = (PickingID)0;
        }

        void setPickingID(PickingID _id)
        {
            VG_ASSERT((PickingID)0 == m_pickingID, "IGraphicInstance \"%s\" already uses PickingID %u", getName().c_str(), m_pickingID);
            m_pickingID = _id;
        }

        PickingID getPickingID() const
        {
            return m_pickingID;
        }

    private:
        PickingID m_pickingID;
    };
}