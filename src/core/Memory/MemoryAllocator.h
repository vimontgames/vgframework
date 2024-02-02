#pragma once

#include <malloc.h> 
#include <new> 

namespace vg::core
{
    class MemoryAllocator
    {
    public:
        static void * New(std::size_t _size);
        static void * New(std::size_t _size, const char * _FileName, int _LineNumber);
        static void * New(std::size_t _size, std::align_val_t _alignment, const char * _FileName, int _LineNumber);

        static void Delete(void * _ptr) noexcept;
        static void Delete(void * _ptr, std::align_val_t _alignment) noexcept;
    };
}