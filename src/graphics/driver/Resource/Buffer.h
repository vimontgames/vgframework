#pragma once

#include "Buffer_consts.h"
#include "core/Object/Object.h"

namespace vg::graphics::driver
{
    namespace base
    {
        class Buffer : public core::Object
        {
        public:
            Buffer(const BufferDesc & _bufDesc, const core::string & _name, void * _initData);
            ~Buffer();

            const BufferDesc & getTexDesc() const { return m_bufDesc; }

        private:
            const BufferDesc m_bufDesc;
        };
    }
}

#include VG_GFXAPI_HEADER(Buffer)

namespace vg::graphics::driver
{
    class Buffer : public VG_GFXAPI::Buffer
    {
        using super = VG_GFXAPI::Buffer;

    public:
        Buffer(const BufferDesc & _bufDesc, const core::string & _name, void * _initData);
        ~Buffer();
    };
}