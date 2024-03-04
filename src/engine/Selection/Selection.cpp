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
        updateSelectionMatrix();
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

        updateSelectionMatrix();
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
        if (remove(_object))
        {
            updateSelectionMatrix();
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool Selection::Add(core::IObject * _object)
    {
        if (add(_object))
        {
            updateSelectionMatrix();
            return true;
        }

        return false;
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
        bool removed = false;
        GameObject * go = dynamic_cast<GameObject *>(_object);
        if (nullptr != go)
        {
            auto children = go->getChildren();
            for (uint j = 0; j < children.size(); ++j)
                removed |= remove(children[j]);
        }

        setSelected(_object, false);
        removed |= m_selection.remove(_object);

        return removed;
    }

    //--------------------------------------------------------------------------------------
    void Selection::setSelected(core::IObject * _object, bool _selected)
    {
        GameObject * go = dynamic_cast<GameObject *>(_object);
        if (nullptr != go)
        {
            go->setRuntimeFlags(IInstance::RuntimeFlags::Selected, _selected);

            auto graphicInstances = go->getGraphicInstances();
            for (uint i = 0; i < graphicInstances.size(); ++i)
            {
                auto instance = graphicInstances[i];
                instance->setRuntimeFlags(IInstance::RuntimeFlags::Selected, _selected);
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

    //--------------------------------------------------------------------------------------
    void Selection::updateSelectionMatrix()
    {
        m_matrix = float4x4::identity();

        auto objects = RemoveChildGameObjectsWithParents(m_selection);

        float3 T = float3(0, 0, 0);

        // average translation, rotate and scale
        uint count = 0;
        for (uint i = 0; i < objects.size(); ++i)
        {
            IGameObject * go = dynamic_cast<IGameObject*>(objects[i]);
            if (go)
            {
                float4x4 m = go->GetGlobalMatrix();
                T.xyz += m[3].xyz;
                ++count;
            }
        }

        T.xyz /= (float)count;

        m_matrix[3].xyz = T.xyz;
    }

    //--------------------------------------------------------------------------------------
    core::float4x4 & Selection::GetSelectionMatrix()
    {
        return m_matrix;
    }

    //--------------------------------------------------------------------------------------
    // Remove objects with parents already in the list
    // This is useful e.g. when deleting or moving several objects
    //--------------------------------------------------------------------------------------
    core::vector<core::IGameObject *> Selection::RemoveChildGameObjectsWithParents(const core::vector<core::IObject *> & _objects) const
    {
        core::vector<core::IGameObject *> gameObjectsWithoutChildrenWithParents;

        for (uint i = 0; i < _objects.size(); ++i)
        {
            IObject * obj = _objects[i];
            IGameObject * go = dynamic_cast<IGameObject *>(obj);
            if (go && !go->IsRoot())
                gameObjectsWithoutChildrenWithParents.push_back(go);
        }

        // Remove objects with parents already in the list
    RESTART:
        for (uint i = 0; i < gameObjectsWithoutChildrenWithParents.size(); ++i)
        {
            IGameObject * goi = dynamic_cast<GameObject*>(gameObjectsWithoutChildrenWithParents[i]);

            if (goi)
            {
                for (uint j = 0; j < gameObjectsWithoutChildrenWithParents.size(); ++j)
                {
                    IGameObject * goj = dynamic_cast<GameObject *>(gameObjectsWithoutChildrenWithParents[j]);

                    if (goj && i != j)
                    {
                        if (goj->HasAncestor(goi))
                        {
                            gameObjectsWithoutChildrenWithParents.remove(goj);
                            goto RESTART;
                        }
                    }
                }
            }
        }

        return gameObjectsWithoutChildrenWithParents;
    }

    //--------------------------------------------------------------------------------------
    core::vector<core::IGameObject *> Selection::DuplicateGameObjects(const core::vector<core::IGameObject *> & _selectedGameObjects)
    {
        core::vector<core::IGameObject *> newGameObjects;

        for (uint i = 0; i < _selectedGameObjects.size(); ++i)
        {
            IGameObject * go = _selectedGameObjects[i];
            IGameObject * parentGameObject = dynamic_cast<IGameObject *>(go->getParent());
            if (nullptr != parentGameObject)
            {
                IGameObject * newGO = (IGameObject *)go->Instanciate();
                const auto index = parentGameObject->GetChildIndex(go);
                newGO->setName(newGO->getName() + " (Copy)");
                parentGameObject->AddChild(newGO, index+1);
                newGameObjects.push_back(newGO);
                VG_SAFE_RELEASE(newGO);
            }
        }

        return newGameObjects;
    }
}