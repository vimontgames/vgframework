#pragma once

#include "core/ISelection.h"

namespace vg::engine
{
    class Selection : public core::ISelection
    {
    public:
        core::IObject * GetSelectedObject   () final override;
        void            SetSelectedObject   (core::IObject * _object) final override;
        ObjectList &    GetSelectedObjects  () final override;
        void            SetSelectedObjects  (const core::vector<core::IObject *> & _objects) final override;
        bool            IsSelectedObject    (core::IObject * _object) final override;
        bool            RemoveFromSelection (core::IObject * _object) final override;
        bool            AddToSelection      (core::IObject * _object) final override;

    private:
        ObjectList      m_selection;
    };
}