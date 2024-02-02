#include "core/Precomp.h"
#include "MemoryAllocator.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    void * MemoryAllocator::New(std::size_t _size)
    {
        void * ptr = std::malloc(_size);
        return ptr;
    }

    //--------------------------------------------------------------------------------------
    void * MemoryAllocator::New(std::size_t _size, const char * _FileName, int _LineNumber)
    {
        void * ptr = std::_malloc_dbg(_size, _NORMAL_BLOCK, _FileName, _LineNumber);
        return ptr;
    }

    //--------------------------------------------------------------------------------------
    void * MemoryAllocator::New(std::size_t _size, std::align_val_t _alignment, const char * _FileName, int _LineNumber)
    {
        void * ptr = _aligned_malloc_dbg(_size, (size_t)_alignment, _FileName, _LineNumber);
        return ptr;
    }

    //--------------------------------------------------------------------------------------
    void MemoryAllocator::Delete(void * _ptr) noexcept
    {
        std::free(_ptr);
    }

    //--------------------------------------------------------------------------------------
    void MemoryAllocator::Delete(void * _ptr, std::align_val_t _alignment) noexcept
    {
        _aligned_free(_ptr);
    }
}