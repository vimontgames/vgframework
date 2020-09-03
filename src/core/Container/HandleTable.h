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

            inline bool isValid() const
            {
                return (index_t)-1 != index;
            }

            inline index_t getIndex() const
            {
                return index;
            }

        private:
            index_t index;
        };

        Handle add(const desc_t & _desc);
        core::u32 remove(Handle & _handle);

    private:
        std::vector<std::pair<desc_t, object_t*>> m_pairs;
    };
}

#include "HandleTable.hpp"