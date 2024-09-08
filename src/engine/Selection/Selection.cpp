#include "engine/Precomp.h"
#include "Selection.h"
#include "renderer/IGraphicInstance.h"
#include "core/GameObject/GameObject.h"
#include "core/Math/Math.h"
#include "core/string/string.h"
#include "core/Instance/Instance.h"

using namespace vg::core;

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    // TODO: shall return the last object added to selection instead?
    //--------------------------------------------------------------------------------------
    IObject * Selection::GetSelectedObject()
    {
        if (m_selectionArray.size() > 0)
            return m_selectionArray[0];
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
        return m_selectionArray;
    }

    //--------------------------------------------------------------------------------------
    void Selection::SetSelectedObjects(const core::vector<core::IObject *> & _objects)
    {
        clear();

        for (auto i = 0; i < _objects.size(); ++i)
            add(_objects[i]);

        updateSelectionMatrix();
    }

    //--------------------------------------------------------------------------------------
    bool Selection::IsSelectedObject(const core::IObject * _object) const
    {
        bool selected = false;
        if (const core::Instance * instance = dynamic_cast<const core::Instance *>(_object))
            selected = asBool(core::ObjectFlags::Selected & instance->getObjectFlags());

        return selected;
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
        if (m_selectionSet.end() == m_selectionSet.find(_object))
        {
            setSelected(_object, true);
            m_selectionArray.push_back(_object);
            m_selectionSet.insert(_object);

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
        VG_ASSERT(m_selectionSet.size() == m_selectionArray.size());

        bool removed = false;
        GameObject * go = dynamic_cast<GameObject *>(_object);
        if (nullptr != go)
        {
            auto children = go->getChildren();
            for (uint j = 0; j < children.size(); ++j)
                removed |= remove(children[j]);
        }

        setSelected(_object, false);

        bool removedFromArray = vector_helper::remove(m_selectionArray, _object);
        bool removedFromHash = m_selectionSet.erase(_object) > 0;

        VG_ASSERT(removedFromArray == removedFromHash);
        removed |= (removedFromArray | removedFromHash);

        return removed;
    }

    //--------------------------------------------------------------------------------------
    void Selection::setSelected(core::IObject * _object, bool _selected)
    {
        GameObject * go = dynamic_cast<GameObject *>(_object);
        if (nullptr != go)
        {
            go->setObjectFlags(ObjectFlags::Selected, _selected);

            auto graphicInstances = go->getGraphicInstances();
            for (uint i = 0; i < graphicInstances.size(); ++i)
            {
                auto instance = graphicInstances[i];
                instance->setObjectFlags(ObjectFlags::Selected, _selected);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void Selection::clear()
    {
        for (uint i = 0; i < m_selectionArray.size(); ++i)
            setSelected(m_selectionArray[i], false);
        m_selectionArray.clear();
        m_selectionSet.clear();
    }

    //--------------------------------------------------------------------------------------
    void Selection::updateSelectionMatrix()
    {
        m_matrix = float4x4::identity();

        auto objects = RemoveChildGameObjectsWithParents(m_selectionArray);

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
    bool Selection::hasAnySelectedAncestor(const core::IObject * _object) const
    {
        IObject * parent = _object->GetParent();

        while (nullptr != parent)
        {
            if (IsSelectedObject(parent))
                return true;

            parent = parent->GetParent();
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    // Remove objects with parents already in the list
    // This is useful e.g. when deleting or moving several objects
    //--------------------------------------------------------------------------------------
    core::vector<core::IGameObject *> Selection::RemoveChildGameObjectsWithParents(const core::vector<core::IObject *> & _objects) const
    {
        core::vector<core::IGameObject *> objectsWithoutParents;
        objectsWithoutParents.reserve(_objects.size());
        for (uint i = 0; i < _objects.size(); ++i)
        {
            IGameObject * go = dynamic_cast<IGameObject *>(_objects[i]);
            if (go && !go->IsRoot())
            {
                if (!hasAnySelectedAncestor(go))
                    objectsWithoutParents.push_back(go);
            }
        }

        return objectsWithoutParents;
    }

    //--------------------------------------------------------------------------------------
    uint extractIndex(const string & _text)
    {
        uint index = 0;

        size_t underscorePos = _text.rfind("_");
        if (underscorePos != string::npos && underscorePos != _text.length() - 1)
        {
            bool allDigits = true;
            for (size_t i = underscorePos + 1; i < _text.length(); ++i)
            {
                if (isdigit(_text[i]))
                {
                    int number = _text[i] - '0';
                    index = index * 10 + number;
                }
                else
                {
                    return -1;
                }
            }
        }

        return index;
    }

    //--------------------------------------------------------------------------------------
    string extractBaseName(const string & _text)
    {
        string name = _text;

        uint number = extractIndex(name);

        if (-1 != number)
        {
            size_t underscorePos = name.rfind("_");
            name = name.substr(0, underscorePos);
        }

        return name;
    }

    //--------------------------------------------------------------------------------------
    core::vector<core::IGameObject *> Selection::DuplicateGameObjects(const core::vector<core::IGameObject *> & _selectedGameObjects)
    {
        core::vector<core::IGameObject *> newGameObjects;

        for (uint i = 0; i < _selectedGameObjects.size(); ++i)
        {
            IGameObject * go = _selectedGameObjects[i];
            IGameObject * parentGameObject = dynamic_cast<IGameObject *>(go->GetParent());
            if (nullptr != parentGameObject)
            {
                IGameObject * newGO = (IGameObject *)go->Instanciate();
                auto index = parentGameObject->GetChildIndex(go);

                // Get unique name and sort
                {
                    string name = newGO->getName();
                    uint number = extractIndex(name);

                    const auto children = parentGameObject->GetChildren();
                    uint highestChildIndex = 0;
                    for (uint j = 0; j < children.size(); ++j)
                    {
                        auto child = children[j];
                        string childName = child->getName();
                        uint childIndex = extractIndex(childName);
                        if (-1 != childIndex)
                        {
                            if (extractBaseName(childName) == extractBaseName(name))
                                highestChildIndex = max(highestChildIndex, childIndex);
                        }
                    }

                    if (-1 == number)
                    {
                        name = fmt::sprintf("%s_%.2u", name, highestChildIndex + 1);
                        newGO->setName(name);
                    }
                    else
                    {
                        name = extractBaseName(name);
                        name = fmt::sprintf("%s_%.2u", name, highestChildIndex + 1);
                        newGO->setName(name);
                    }

                    for (uint j = 0; j < children.size(); ++j)
                    {
                        auto child = children[j];
                        string childName = child->getName();
                        uint childIndex = extractIndex(childName);
                        if (-1 != childIndex)
                        {
                            if (extractBaseName(childName) == extractBaseName(name))
                            {
                                if (childIndex == highestChildIndex)
                                    index = j;
                            }
                        }
                    }
                }

                parentGameObject->AddChild(newGO, index+1);
                newGameObjects.push_back(newGO);
                VG_SAFE_RELEASE(newGO);
            }
        }

        return newGameObjects;
    }
}