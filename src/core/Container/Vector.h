#pragma once

//--------------------------------------------------------------------------------------
// Helper functions for std::vector
//--------------------------------------------------------------------------------------

namespace vg::core
{
    namespace vector_helper
    {
        //--------------------------------------------------------------------------------------
        template <typename T> inline bool exists(const std::vector<T>& _vector, const T& _element)
        {
            return std::find(cbegin(_vector), cend(_vector), _element) != cend(_vector);
        }

        //--------------------------------------------------------------------------------------
        template <typename T> inline bool addUnique(std::vector<T> & _vector, const T & _element)
        {
            if (exists(_vector, _element))
                return false;

            _vector.push_back(_element);
    
            return true;
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
