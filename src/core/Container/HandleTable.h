#pragma once

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    // A table of pointers to objects of type object_t *
    // Created from a descriptor of type desc_t
    // Identified with handles using type U to store an index
    //--------------------------------------------------------------------------------------
    template <typename desc_t, class object_t, typename index_t> class HandleTable
    {
    public:
        HandleTable();
        ~HandleTable();

        struct Handle
        {
            inline Handle(index_t _index = (index_t)-1) :
                index(_index)
            {
      
            }

            inline void reset()
            {
                index = (index_t)-1;
            }

            inline bool isValid() const
            {
                return (index_t)-1 != index;
            }

            inline index_t getIndex() const
            {
                return index;
            }

            inline bool operator == (const Handle & _other) const
            {
                return _other.index == index;
            }

            inline bool operator != (const Handle & _other) const
            {
                return _other.index != index;
            }

            struct hash
            {
                inline core::size_t operator() (const Handle & _handle) const
                {
                    return core::hash<index_t>()(_handle.index);
                }
            };

        private:
            index_t index;
        };

        Handle add(const desc_t & _desc);
        object_t * get(const Handle & _handle) const;
        core::u32 remove(Handle & _handle);

    private:
        core::vector<core::pair<desc_t, object_t*>> m_pairs;
    };
}

#include "HandleTable.hpp"