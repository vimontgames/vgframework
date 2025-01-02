#pragma once

#if VG_ENABLE_GLOBAL_NEW_DELETE
#pragma push_macro("new")
#undef new

#include "MemoryAllocator.h"

//--------------------------------------------------------------------------------------
void * operator new(std::size_t _size)
{
    return vg::core::MemoryAllocator::New(_size, nullptr, 0);
}

//--------------------------------------------------------------------------------------
void operator delete(void * ptr) noexcept
{
    vg::core::MemoryAllocator::Delete(ptr);
}

//--------------------------------------------------------------------------------------
void * operator new(std::size_t _size, const char * _file, int _line)
{
    return vg::core::MemoryAllocator::New(_size, _file, _line);
}

//--------------------------------------------------------------------------------------
void operator delete(void * ptr, const char * _file, int _line) noexcept
{
    vg::core::MemoryAllocator::Delete(ptr);
}

//--------------------------------------------------------------------------------------
void * operator new(std::size_t _size, std::align_val_t _alignment, const char * _file, int _line)
{
    return vg::core::MemoryAllocator::New(_size, _alignment, _file, _line);
}

//--------------------------------------------------------------------------------------
void operator delete(void * ptr, std::align_val_t _alignment, const char * _file, int _line) noexcept
{
    vg::core::MemoryAllocator::Delete(ptr, _alignment);
}

//--------------------------------------------------------------------------------------
void operator delete(void * ptr, std::align_val_t _alignment) noexcept
{
    vg::core::MemoryAllocator::Delete(ptr, _alignment);
}

//--------------------------------------------------------------------------------------
void * operator new[](std::size_t _size, const char * _file, int _line)
{
    return vg::core::MemoryAllocator::New(_size, _file, _line);
}

//--------------------------------------------------------------------------------------
void operator delete[](void * _ptr, const char * _file, int _line) noexcept
{
    vg::core::MemoryAllocator::Delete(_ptr);
}

//--------------------------------------------------------------------------------------
void operator delete[](void * _ptr) noexcept
{
    vg::core::MemoryAllocator::Delete(_ptr);
}

#pragma pop_macro("new")
#endif