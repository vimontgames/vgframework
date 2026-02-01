#pragma once

#include "core/IObjectList.h"
#include "core/IClassDesc.h"

namespace vg::core
{
    template <class T> class ObjectList : public IObjectList
    {
    public:
        VG_CLASS_DECL(ObjectList, IObjectList);

        //--------------------------------------------------------------------------------------
        ObjectList(const core::string & _name, core::IObject * _parent) :
            super(_name, _parent)
        {
            m_objects.reserve(256);
        }

        //--------------------------------------------------------------------------------------
        ~ObjectList()
        {

        }

        //--------------------------------------------------------------------------------------
        bool Add(const string & _name) override
        {
            T & obj = m_objects.emplace_back();
            obj.SetName(_name);
            obj.RegisterUID();

            for (auto & res : m_objects)
                res.SetParent(this);

            return true;
        }

        //--------------------------------------------------------------------------------------
        bool Pop() override
        {
            if (m_objects.size() > 0)
            {
                m_objects.pop_back();
                return true;
            }
            return false;
        }

        //--------------------------------------------------------------------------------------
        bool MoveUp(size_t _index) override
        {
            if (_index > 0 && _index < m_objects.size())
            {
                m_objects[_index].Swap(&m_objects[_index - 1]);
                return true;
            }
            return false;
        }

        //--------------------------------------------------------------------------------------
        bool MoveDown(size_t _index) override
        {
            if (_index + 1 < m_objects.size())
            {
                m_objects[_index].Swap(&m_objects[_index + 1]);
                return true;
            }
            return false;
        }

        //--------------------------------------------------------------------------------------
        bool Remove(core::size_t _index) override
        {
            if (_index < m_objects.size())
            {
                m_objects.erase(m_objects.begin() + _index);
                return true;
            }
            return false;
        }

        //--------------------------------------------------------------------------------------
        size_t Size() const override
        {
            return m_objects.size();
        }

        //--------------------------------------------------------------------------------------
        const core::vector<T> & getObjects() const
        {
            return m_objects;
        }

        //--------------------------------------------------------------------------------------
        core::vector<T> & getObjects()
        {
            return m_objects;
        }

        //--------------------------------------------------------------------------------------
        void OnEnable() override
        {
            super::OnEnable();
            for (auto i = 0; i < m_objects.size(); ++i)
                m_objects[i].OnEnable();
        }

        //--------------------------------------------------------------------------------------
        void OnDisable() override
        {
            for (auto i = 0; i < m_objects.size(); ++i)
                m_objects[i].OnDisable();
            super::OnDisable();
        }

        //--------------------------------------------------------------------------------------
        bool CanAdd() const override
        {
            return m_objects.size() < m_maxSize;
        }

        //--------------------------------------------------------------------------------------
        bool CanRemove() const override
        {
            return m_objects.size() > m_minSize;
        }

        //--------------------------------------------------------------------------------------
        void SetMinSize(size_t _count) override
        {
            m_minSize = _count;
        }

        //--------------------------------------------------------------------------------------
        size_t GetMinSize() const override
        {
            return m_minSize;
        }

        //--------------------------------------------------------------------------------------
        void SetMaxSize(size_t _count) override
        {
            m_maxSize = _count;
        }

        //--------------------------------------------------------------------------------------
        size_t GetMaxSize() const override
        {
            return m_maxSize;
        }

    protected:
        vector<T> m_objects;
        size_t m_minSize = 0;
        size_t m_maxSize = (size_t)-1;
    };

    //--------------------------------------------------------------------------------------
    template <class T> bool ObjectList<T>::registerProperties(vg::core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyObjectVector(ObjectList<T>, m_objects, T, "Objects");

        return true;
    }
}