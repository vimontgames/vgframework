#pragma once

#include "Buffer_consts.h"
#include "gfx/IBuffer.h"
#include "gfx/Resource/Resource.h"
#include "gfx/BindlessTable/BindlessTable_consts.h"

namespace vg::gfx
{
    namespace base
    {
        class Buffer : public IBuffer
        {
        public:
            VG_CLASS_DECL_PASSTHROUGH(Buffer, IBuffer);

            Buffer(const BufferDesc & _bufDesc, const core::string & _name, const void * _initData);
            ~Buffer();

            VG_INLINE const BufferDesc &                    getBufDesc          () const;
            VG_INLINE const gfx::Resource &                 getResource         () const;
            VG_INLINE gfx::Resource &                       getResource         ();
            VG_INLINE const gfx::BindlessBufferHandle &     getBufferHandle     () const;
            VG_INLINE const gfx::BindlessRWBufferHandle &   getRWBufferHandle   () const;

        protected:
            const BufferDesc                                m_bufDesc;
            gfx::Resource                                   m_resource;
            gfx::BindlessBufferHandle                       m_bufferHandle;
            gfx::BindlessRWBufferHandle                     m_rwBufferHandle;
        };
    }
}

#ifdef VG_DX12
#include "dx12/Buffer_dx12.h"
#elif defined(VG_VULKAN)
#include "vulkan/Buffer_vulkan.h"
#else
#error Undefined GFXAPI
#endif

namespace vg::gfx
{
    class Buffer : public VG_GFXAPI::Buffer
    {
        using super = VG_GFXAPI::Buffer;

    public:
        const char * GetClassName() const final { return "Buffer"; }

        Buffer(const BufferDesc & _bufDesc, const core::string & _name, const void * _initData = nullptr, ReservedSlot _reservedSlot = ReservedSlot::None);
        ~Buffer();
    };
}

#if VG_ENABLE_INLINE
#include "Buffer.inl"
#endif