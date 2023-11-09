#pragma once

#include "core/Instance/Instance.h"

namespace vg::gfx
{
    class CommandList;
}

namespace vg::renderer
{
    class RenderContext;
    using PickingID = core::uint;

    class IGraphicInstance : public core::Instance
    {
    public:
        IGraphicInstance(const core::string & _name, core::IObject * _parent) :
            core::Instance(_name, _parent)
        {

        }

        virtual void Draw(const RenderContext & _renderContext, gfx::CommandList * _cmdList) const = 0;

        void setPickingID(PickingID _id)
        {
            VG_ASSERT((PickingID)0 == m_pickingID, "IGraphicInstance \"%s\" already uses PickingID %u", getName(), m_pickingID);
            m_pickingID = _id;
        }

        PickingID getPickingID() const
        {
            return m_pickingID;
        }

    private:
        PickingID m_pickingID = (PickingID)0;
    };
}