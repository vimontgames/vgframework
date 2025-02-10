#pragma once

#include <optional>

namespace vg::core
{
    namespace vector_helper
    {
        //--------------------------------------------------------------------------------------
        // Helper funcs for std::vector
        //--------------------------------------------------------------------------------------

        //--------------------------------------------------------------------------------------
        template <typename T> inline std::optional<T&> find(const std::vector<T>& _vector, const T& _element)
        {
            if (auto it{ std::find(cbegin(_vector), cend(_vector), _element) }; it != cend(_vector))
                return *it;
            return {};
        }

        //--------------------------------------------------------------------------------------
        template <typename T> inline bool exists(const std::vector<T>& _vector, const T& _element)
        {
            return std::find(cbegin(_vector), cend(_vector), _element) != cend(_vector);
        }

        //--------------------------------------------------------------------------------------
        template <typename T> inline bool remove(std::vector<T>& _vector, const T& _element)
        {
            if (auto it{ std::remove_if(begin(_vector), end(_vector), [=](auto s) { return s == _element; }) }; it != end(_vector))
            {
                _vector.erase(it);
                return true;
            }
            return false;
        }
    }
}
