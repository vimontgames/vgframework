namespace vg::core
{
    //--------------------------------------------------------------------------------------
    template <typename desc_t, class object_t, typename index_t> HandleTable<desc_t,object_t,index_t>::HandleTable()
    {

    }

    //--------------------------------------------------------------------------------------
    template <typename desc_t, class object_t, typename index_t> HandleTable<desc_t,object_t,index_t>::~HandleTable()
    {

    }

    //--------------------------------------------------------------------------------------
    template <typename desc_t, class object_t, typename index_t> typename HandleTable<desc_t,object_t,index_t>::Handle HandleTable<desc_t,object_t,index_t>::add(const desc_t & _desc)
    {
        // find existing
        {
            uint index = 0;
            for (auto & pair : m_pairs)
            {
                if (pair.first == _desc)
                {
                    VG_ASSERT(pair.second);
                    pair.second->addRef();
                    return index;
                }
                ++index;
            }
        }

        // reuse empty slot
        {
            uint index = 0;
            for (auto & pair : m_pairs)
            {
                if (nullptr == pair.second)
                {
                    pair.first = _desc;
                    pair.second = new object_t(_desc);
                    return index;
                }
                ++index;
            }
        }

        // add
        {
            uint index = (uint)m_pairs.size();
            m_pairs.push_back(pair<desc_t, object_t*>(_desc, new object_t(_desc)));
            return index;
        }
    }

    //--------------------------------------------------------------------------------------
    template <typename desc_t, class object_t, typename index_t> object_t * HandleTable<desc_t, object_t, index_t>::get(const Handle & _handle) const
    {
        VG_ASSERT(_handle.isValid());

        if (_handle.isValid())
        {
            auto & pair = m_pairs[_handle.getIndex()];
            VG_ASSERT(pair.second);
            return pair.second;
        }
 
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    template <typename desc_t, class object_t, typename index_t> core::uint HandleTable<desc_t,object_t,index_t>::remove(typename HandleTable<desc_t, object_t, index_t>::Handle & _handle)
    {
        const char * name = strrchr(typeid(object_t).name(), ':');
        VG_ASSERT(_handle.isValid(), "Invalid '%s' Handle", name ? name+1 : typeid(object_t).name());
        uint refCount = 0;

        if (_handle.isValid())
        {
            auto & pair = m_pairs[_handle.getIndex()];
            refCount = pair.second->release();
            if (refCount == 0)
            {
                pair.first = {};
                pair.second = nullptr;
            }
        }

        _handle.reset();
        return refCount;
    }
}