#pragma once

namespace vg::core
{
    template <typename E> class EnumHelper
    {
        using T = magic_enum::underlying_type_t<E>;

    public:
        EnumHelper()
        {
            const auto entries = magic_enum::enum_entries<E>();
            const auto prout = magic_enum::enum_values<E>();
            m_count = (uint)entries.size();
            m_values = (T*)malloc(sizeof(T)*m_count);
            for (uint e = 0; e < m_count; ++e)
            {
                m_values[e] = (T)entries[e].first;
                m_names += (string)entries[e].second + '\0';
            }
            m_names += '\0';
        }

        ~EnumHelper()
        {
            VG_SAFE_FREE(m_values);
        }

        const u32 getCount() const
        {
            return m_count;
        }

        const char * getNames() const
        {
            return m_names.c_str();
        }

        const T * getValues() const
        {
            return m_values;
        }

    private:
        u32 m_count = 0;
        string m_names;
        T * m_values = nullptr;
    };
}