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
            using super = IBuffer;

        public:
            Buffer(const BufferDesc & _bufDesc, const core::string & _name, const void * _initData);
            ~Buffer();

            const BufferDesc & getBufDesc() const { return m_bufDesc; }

            const gfx::Resource & getResource() const { return m_resource; }
            gfx::Resource & getResource() { return m_resource; }

            const gfx::BindlessBufferHandle getBufferHandle() const { return m_bufferHandle; }

        protected:
            const BufferDesc                    m_bufDesc;
            gfx::Resource                       m_resource;
            gfx::BindlessBufferHandle           m_bufferHandle;
        };
    }
}

#include VG_GFXAPI_HEADER(Buffer)

namespace vg::gfx
{
    class Buffer : public VG_GFXAPI::Buffer
    {
        using super = VG_GFXAPI::Buffer;

    public:
        const char * getClassName() const final { return "Buffer"; }

        Buffer(const BufferDesc & _bufDesc, const core::string & _name, const void * _initData = nullptr, ReservedSlot _reservedSlot = ReservedSlot::None);
        ~Buffer();
    };
}