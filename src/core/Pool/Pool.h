#pragma once

namespace vg::core
{
    template <class T> class Pool
    {
    public:
        static constexpr uint elemCountPerPage = 64;
        using elemMask = u64;

        T * alloc()
        {
            for (auto & page : m_pages)
            {
                if (0 != page.mask)
                {
                    u32 objIndex;
                    _BitScanForward64((DWORD*)&objIndex, page.mask);
                    T * ptr = (T*)&page.data[objIndex * sizeof(T)];
                    page.mask &= ~(elemMask(1) << objIndex);
                    return ptr;
                }
            }

            m_pages.resize(m_pages.size() + 1);
            Page & page = m_pages.back();
            T * ptr = (T*)&page.data[0];
            page.mask &= ~1;
            return ptr;
        }

        void dealloc(T * _ptr)
        {
            const T * start = (T*)&m_pages[0].data[0];
            uint_ptr pageIndex = (uint_ptr(_ptr)-uint_ptr(start)) / sizeof(Page);
            uint_ptr objIndex = (uint_ptr(_ptr) - uint_ptr(&m_pages[pageIndex])) / sizeof(T);
            T * ptr = (T*)&m_pages[pageIndex].data[objIndex * sizeof(T)];
            m_pages[pageIndex].mask |= elemMask(1) << objIndex;
        }

        struct Page
        {
            elemMask    mask = (elemMask)-1;
            char        data[sizeof(T)*elemCountPerPage];
        };

    private:
        core::vector<Page>	m_pages;
    };
}