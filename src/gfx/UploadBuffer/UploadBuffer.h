#pragma once

#include "core/Object/Object.h"
#include "core/Scheduler/Mutex.h"

namespace vg::gfx
{
    class CommandList;
    class Buffer;
    class Texture;

    vg_enum_class(vg::gfx, RingAllocCategory, core::u8,
        Buffer = 0,
        Texture
    );

    struct Alloc
    {
        inline Alloc()
        {

        }

        inline Alloc(core::uint_ptr _size) :
            begin(_size),
            end(0)
        {

        }

        inline Alloc(core::uint_ptr _begin, core::uint_ptr _end) :
            begin(_begin),
            end(_end)
        {

        }

        inline void grow(const Alloc & _alloc)
        {
            begin = begin < _alloc.begin ? begin : _alloc.begin; // min
            end = end > _alloc.end ? end : _alloc.end;           // max
        }

        core::uint_ptr begin, end;
    };

    class UploadBuffer : public core::Object
    {
    public:
        using super = core::Object;

        UploadBuffer(const core::string & _name, core::size_t _size, core::uint _index);
        ~UploadBuffer();

        core::u64               getAvailableUploadSize  ();
        core::u64               getAvailableUploadSize  (Alloc & _bestRange);
        void                    setNextAllocSize        (core::size_t _size);  
        core::u64               getTotalSize            ();
        core::size_t            getAlignedSize          (core::size_t _size, core::size_t  _alignment) const;

        core::u8 *              map                     (Buffer * _buffer, core::size_t _size, core::size_t _aligment);
        core::u8 *              map                     (Texture * _buffer, core::size_t _size, core::size_t _aligment);

        void                    unmap                   (Buffer * _buffer, core::u8 * _dst, size_t _size);
        void                    unmap                   (Texture * _texture, core::u8 * _dst, size_t _size);

        void                    flush                   (CommandList * _cmdList, bool _canBeEmpty = false);
        void                    sync                    ();

        VG_INLINE Buffer *      getBuffer               () const;

        bool                    isAvailable             (core::size_t _size, core::size_t _alignment);

    private:
        VG_INLINE core::u8 *    getBaseAddress          () const;

    private:
        void                    upload                  (Texture * _dst, core::uint_ptr _from, size_t _size);
        void                    upload                  (Buffer * _dst, core::uint_ptr _from, size_t _size);
        core::u8 *              map                     (core::size_t _size, core::size_t _aligment, RingAllocCategory _category);
        core::uint_ptr          alloc                   (core::size_t _size, core::size_t _alignment, RingAllocCategory _category);
        bool                    tryAllocate             (core::size_t _size, core::size_t _alignment, Alloc * _alloc);
        core::uint              isOverlaping            (const Alloc & _alloc, const core::vector<Alloc> & _ranges) const;
        void                    lock                    ();
        void                    unlock                  ();

    private:
        core::uint              m_index;
        Buffer *                m_buffer                = nullptr;
        core::u8 *              m_begin                 = nullptr;
        core::uint_ptr          m_currentOffset         = 0;
        core::vector<Alloc>     m_ranges;               // Current frame allocation ranges
        core::vector<Alloc>     m_previousRanges;       // Previous frame allocation ranges
        core::size_t            m_nextAllocSize         = (core::size_t)-1;

        // Mutex is only necessary for main upload command buffer, other can just check
        core::RecursiveMutex    m_mutex                 = core::RecursiveMutex("RecursiveMutex - UploadBuffer");
        core::AssertMutex       m_assertMutex           = core::AssertMutex("AssertMutex - UploadBuffer");

        struct ResourceUploadInfo
        {
            ResourceUploadInfo(core::uint_ptr _offset, size_t _size = (size_t)-1) :
                offset(_offset),
                size(_size)
            {

            }
            core::uint_ptr offset;
            size_t size;
        };

        core::vector<core::pair<Texture *, ResourceUploadInfo>>  m_texturesToUpload;
        core::vector<core::pair<Buffer *, ResourceUploadInfo>>   m_buffersToUpload;
    };
}

#if VG_ENABLE_INLINE
#include "UploadBuffer.inl"
#endif