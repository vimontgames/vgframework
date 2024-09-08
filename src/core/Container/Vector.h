#pragma once

namespace vg::core
{
    namespace vector_helper
    {
        //--------------------------------------------------------------------------------------
        // Helper funcs for std::vector
        //--------------------------------------------------------------------------------------

        //--------------------------------------------------------------------------------------
        template <typename T> inline T & find(const std::vector<T> & _vector, const T & _element)
        {
            if (auto it = std::find(this->begin(), this->end(), _element) != this->end())
                return *it;
            else
                return nullptr;
        }

        //--------------------------------------------------------------------------------------
        template <typename T> inline bool exists(const std::vector<T> & _vector, const T & _element)
        {
            return find(_vector.begin(), _vector.end(), _element) != _vector.end();
        }

        //--------------------------------------------------------------------------------------
        template <typename T> inline bool remove(std::vector<T> & _vector, const T & _element)
        {
            auto end = std::remove_if(_vector.begin(), _vector.end(), [=](auto s) { return s == _element; });
            if (end != _vector.end())
            {
                _vector.erase(end);
                return true;
            }
            return false;
        }
    }
}