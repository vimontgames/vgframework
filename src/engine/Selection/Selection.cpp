#include "engine/Precomp.h"
#include "Selection.h"

using namespace vg::core;

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    IObject * Selection::GetSelectedObject()
    {
        if (m_selection.count() > 0)
            return m_selection[0];
        else
            return nullptr;
    }

    //--------------------------------------------------------------------------------------
    void Selection::SetSelectedObject(core::IObject * _object)
    {
        m_selection.clear();
        m_selection.push_back(_object);
    }

    //--------------------------------------------------------------------------------------
    ISelection::ObjectList & Selection::GetSelectedObjects()
    {
        return m_selection;
    }

    //--------------------------------------------------------------------------------------
    void Selection::SetSelectedObjects(const ObjectList & _objects)
    {
        m_selection = _objects;
    }

    //--------------------------------------------------------------------------------------
    bool Selection::IsSelectedObject(core::IObject * _object)
    {
        for (int i = 0; i < m_selection.size(); ++i)
        {
            if (_object == m_selection[i])
                return nullptr != m_selection[i];
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool Selection::RemoveFromSelection(core::IObject * _object)
    {
        return m_selection.remove(_object);
    }

    //--------------------------------------------------------------------------------------
    bool Selection::AddToSelection(core::IObject * _object)
    {
        if (!m_selection.exists(_object))
        {
            m_selection.push_back(_object);
            return true;
        }
        return false;
    }
}