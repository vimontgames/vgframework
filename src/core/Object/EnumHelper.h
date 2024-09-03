#pragma once

#include "core/Types/Traits.h"

namespace vg::core
{
    template <typename E> class EnumHelper
    {
        using T = typename std::underlying_type<E>::type;

    public:
        EnumHelper()
        {
            const auto entries = enumPairs<E>();
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

        static const uint getStaticCount()
        {
            return (uint)enumPairs<E>().size();
        }

        static const uint getSizeOfUnderlyingType()
        {
            return sizeof(T);
        }

        static const string getStaticNames()
        {
            string names;
            const auto entries = enumPairs<E>();
            uint count = (uint)entries.size();
            for (uint e = 0; e < count; ++e)
                names += (string)entries[e].second + '\0';
            
            names += '\0';

            return names;
        }

        static const vector<T> getStaticValues()
        {
            vector<T> values;
            const auto entries = enumPairs<E>();;
            uint count = (uint)entries.size();
            for (uint e = 0; e < count; ++e)
            {
                auto value = (T)entries[e].first;
                if (scalarTraits<T>::is_signed)
                    values.push_back(-(~(typename scalarTraits<T>::signed_type)value)-1);   
                else
                    values.push_back(value);
            }

            return values;
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