#pragma once

#include "engine/IResourceList.h"

namespace vg::engine
{
    template <class T> class ResourceList : public IResourceList
    {
    public:
        VG_CLASS_DECL(ResourceList, IResourceList);

        ResourceList(const core::string & _name, core::IObject * _parent) :
            super(_name, _parent)
        {
            m_resources.reserve(256);
        }

        ~ResourceList()
        {

        }

        //--------------------------------------------------------------------------------------
        bool Add() override
        {
            T & res = m_resources.emplace_back();

            for (auto & res : m_resources)
                res.SetParent(this);

            return true;
        }

        //--------------------------------------------------------------------------------------
        bool Remove() override
        {
            if (m_resources.size() > 0)
            {
                m_resources.pop_back();

                for (auto & res : m_resources)
                    res.SetParent(this);

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
        void OnEnable()
        {
            super::OnEnable();
            for (auto i = 0; i < m_resources.size(); ++i)
                m_resources[i].OnEnable();
        }

        //--------------------------------------------------------------------------------------
        void OnDisable()
        {
            for (auto i = 0; i < m_resources.size(); ++i)
                m_resources[i].OnDisable();
            super::OnDisable();
        }

    protected:
        core::vector<T> m_resources;
    };

    //--------------------------------------------------------------------------------------
    template <class T> bool ResourceList<T>::registerProperties(vg::core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyObjectVectorEx(ResourceList<T>, m_resources, T, "Resources", core::PropertyFlags::Resource);

        return true;
    }
}