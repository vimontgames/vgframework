#pragma once

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    // A pool to allocate indexes
    //--------------------------------------------------------------------------------------
    template <typename T, core::size_t count> class IndexPool
    {
    public:
        static constexpr uint elemCountPerPage = 64;
        static constexpr uint pageCount = (count + elemCountPerPage - 1) / elemCountPerPage;
        using elemMask = u64;

        T alloc()
        {
            m_used++;
            u32 pageIndex = 0;
            for (auto & page : m_pages)
            {
                if (0 != page.mask)
                {
                    u32 objIndex;
                    _BitScanForward64((DWORD*)&objIndex, page.mask);
                    page.mask &= ~(elemMask(1) << objIndex);
                    return pageIndex * elemCountPerPage + objIndex;
                }
                ++pageIndex;
            }

            --m_used;
            VG_ASSERT(false);
            return (T)-1;
        }

        bool isAvailable(T _index)
        {
            const uint pageIndex = _index / pageCount;
            const uint localIndex = _index - (pageIndex * elemCountPerPage);
            return (elemMask(1) << localIndex) & m_pages[pageIndex].mask;
        }

        void free(T _index)
        {
            const uint pageIndex = _index / pageCount;
            const uint localIndex = _index - (pageIndex * elemCountPerPage);
            m_pages[pageIndex].mask |= elemMask(1) << localIndex;
            --m_used;
        }

        struct Page
        {
            void clear()
            {
                mask = (elemMask)-1;
            }

            elemMask mask = (elemMask)-1;
        };

        constexpr core::size_t allocated() const
        {
            return count;
        }

        const core::size_t used() const
        {
            return m_used;
        }

        const core::size_t available() const
        {
            return count - m_used;
        }

        void clear()
        {
            for (uint i = 0; i < pageCount; ++i)
                m_pages[i].clear();
        }

    private:
        Page            m_pages[pageCount];
        core::size_t    m_used = 0;
    };
}