#pragma once

namespace vg::core
{
    template <typename T> class vector : public std::vector<T>
    {
    public:
        //--------------------------------------------------------------------------------------
        vector() : std::vector<T>()
        {
        }

        //--------------------------------------------------------------------------------------
        vector(typename std::vector<T>::size_type _count) : std::vector<T>(_count)
        {
        }

        //--------------------------------------------------------------------------------------
        uint count() const
        {
            return (uint)this->size();
        }

        //--------------------------------------------------------------------------------------
        bool exists(const typename T & element) const
        {
            return std::find(this->begin(), this->end(), element) != this->end();
        }

        //--------------------------------------------------------------------------------------
        bool remove(const typename T & element)
        {
            auto end = std::remove_if(this->begin(), this->end(), [=](auto s) { return s == element; });
            if (end != this->end())
            {
                this->erase(end);
                return true;
            }
            return false;
        }
    };
}