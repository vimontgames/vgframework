#pragma once

#include "core/ISelection.h"

namespace vg::engine
{
    class Selection : public core::ISelection
    {
    public:
        core::IObject *                 GetSelectedObject   () final override;
        core::vector<core::IObject *> & GetSelectedObjects  () final override;

        void                            SetSelectedObject   (core::IObject * _object) final override;
        void                            SetSelectedObjects  (const core::vector<core::IObject *> & _objects) final override;

        bool                            IsSelectedObject    (core::IObject * _object) final override;

        bool                            Add                 (core::IObject * _object) final override;
        bool                            Remove              (core::IObject * _object) final override;

        void                            Clear               () final override;

    protected:
        bool                            add                 (core::IObject * _object);
        bool                            remove              (core::IObject * _object);
        void                            setSelected         (core::IObject * _object, bool _selected);
        void                            clear               ();

    private:
        core::vector<core::IObject *>   m_selection;
    };
}