#pragma once

#include "core/IResourceList.h"

namespace vg::core
{
    template <class T> class ResourceList : public IResourceList
    {
    public:
        VG_CLASS_DECL_TEMPLATE(ResourceList, IResourceList);

        //--------------------------------------------------------------------------------------
        ResourceList(const core::string & _name, core::IObject * _parent) :
            super(_name, _parent)
        {
            m_resources.reserve(256);
        }

        //--------------------------------------------------------------------------------------
        ~ResourceList()
        {

        }

        //--------------------------------------------------------------------------------------
        bool Add(const string & _name, const string & _path, ResourceUserData _userData) override
        {
            T & res = m_resources.emplace_back();
            res.SetName(_name);
            res.RegisterUID();
            res.SetUserData(_userData);

            for (auto & res : m_resources)
                res.SetParent(this);

            if (!_path.empty())
                res.SetResourcePath(_path);

            return true;
        }

        //--------------------------------------------------------------------------------------
        bool Pop() override
        {
            if (m_resources.size() > 0)
            {
                m_resources.pop_back();
                return true;
            }
            return false;
        }

        //--------------------------------------------------------------------------------------
        bool MoveUp(size_t _index) override
        {
            if (_index > 0 && _index < m_resources.size())
            {
                m_resources[_index].Swap(&m_resources[_index - 1]);
                return true;
            }
            return false;
        }

        //--------------------------------------------------------------------------------------
        bool MoveDown(size_t _index) override
        {
            if (_index + 1 < m_resources.size())
            {
                m_resources[_index].Swap(&m_resources[_index + 1]);
                return true;
            }
            return false;
        }

        //--------------------------------------------------------------------------------------
        bool Remove(core::size_t _index) override
        {
            if (_index < m_resources.size())
            {
                m_resources.erase(m_resources.begin() + _index);
                return true;
            }
            return false;
        }

        //--------------------------------------------------------------------------------------
        size_t Size() const override
        {
            return m_resources.size();
        }

        //--------------------------------------------------------------------------------------
        const core::vector<T> & getResources() const
        {
            return m_resources;
        }

        //--------------------------------------------------------------------------------------
        core::vector<T> & getResources()
        {
            return m_resources; 
        }

        //--------------------------------------------------------------------------------------
        bool CanAdd() const override
        {
            return m_resources.size() < m_maxSize;
        }

        //--------------------------------------------------------------------------------------
        bool CanRemove() const override
        {
            return m_resources.size() > m_minSize;
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

        //--------------------------------------------------------------------------------------
        void OnEnable() override
        {
            super::OnEnable();
            for (auto i = 0; i < m_resources.size(); ++i)
                m_resources[i].OnEnable();
        }

        //--------------------------------------------------------------------------------------
        void OnDisable() override
        {
            for (auto i = 0; i < m_resources.size(); ++i)
                m_resources[i].OnDisable();
            super::OnDisable();
        }

    protected:
        core::vector<T> m_resources;
        size_t m_minSize = 0;
        size_t m_maxSize = (size_t)-1;
    };

    //--------------------------------------------------------------------------------------
    template <class T> bool ResourceList<T>::registerProperties(vg::core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyObjectVectorEx(ResourceList<T>, m_resources, T, "File", core::PropertyFlags::Resource);

        return true;
    }
}