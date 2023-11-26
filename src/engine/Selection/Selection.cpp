#include "engine/Precomp.h"
#include "Selection.h"
#include "renderer/IGraphicInstance.h"
#include "core/GameObject/GameObject.h"

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
        clear();
        add(_object);
    }

    //--------------------------------------------------------------------------------------
    core::vector<core::IObject *> & Selection::GetSelectedObjects()
    {
        return m_selection;
    }

    //--------------------------------------------------------------------------------------
    void Selection::SetSelectedObjects(const core::vector<core::IObject *> & _objects)
    {
        clear();

        for (uint i = 0; i < _objects.size(); ++i)
            add(_objects[i]);
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
    bool Selection::Remove(core::IObject * _object)
    {
        return remove(_object);
    }

    //--------------------------------------------------------------------------------------
    bool Selection::Add(core::IObject * _object)
    {
        return add(_object);
    }

    //--------------------------------------------------------------------------------------
    void Selection::Clear()
    {
        clear();
    }

    //--------------------------------------------------------------------------------------
    bool Selection::add(core::IObject * _object)
    {
        if (!m_selection.exists(_object))
        {
            setSelected(_object, true);
            m_selection.push_back(_object);

            GameObject * go = dynamic_cast<GameObject *>(_object);
            if (nullptr != go)
            {
                auto children = go->getChildren();
                for (uint j = 0; j < children.size(); ++j)
                    add(children[j]);
            }

            return true;
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool Selection::remove(core::IObject * _object)
    {
        GameObject * go = dynamic_cast<GameObject *>(_object);
        if (nullptr != go)
        {
            auto children = go->getChildren();
            for (uint j = 0; j < children.size(); ++j)
                remove(children[j]);
        }

        setSelected(_object, false);
        return m_selection.remove(_object);
    }

    //--------------------------------------------------------------------------------------
    void Selection::setSelected(core::IObject * _object, bool _selected)
    {
        GameObject * go = dynamic_cast<GameObject *>(_object);
        if (nullptr != go)
        {
            go->SetFlags(IInstance::Selected, _selected);

            auto graphicInstances = go->getGraphicInstances();
            for (uint i = 0; i < graphicInstances.size(); ++i)
            {
                auto instance = graphicInstances[i];
                instance->SetFlags(IInstance::Selected, _selected);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void Selection::clear()
    {
        for (uint i = 0; i < m_selection.size(); ++i)
            setSelected(m_selection[i], false);
        m_selection.clear();
    }
}