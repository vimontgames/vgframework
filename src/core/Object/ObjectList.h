#pragma once

#include "core/IObjectList.h"

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
        bool RemoveAt(core::size_t _index) override
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
        void OnEnable()
        {
            super::OnEnable();
            for (auto i = 0; i < m_objects.size(); ++i)
                m_objects[i].OnEnable();
        }

        //--------------------------------------------------------------------------------------
        void OnDisable()
        {
            for (auto i = 0; i < m_objects.size(); ++i)
                m_objects[i].OnDisable();
            super::OnDisable();
        }

    protected:
        core::vector<T> m_objects;
    };

    //--------------------------------------------------------------------------------------
    template <class T> bool ObjectList<T>::registerProperties(vg::core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyObjectVector(ObjectList<T>, m_objects, T, "Objects");

        return true;
    }
}