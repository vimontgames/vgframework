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

            const driver::Resource & getResource() const { return m_resource; }
            driver::Resource & getResource() { return m_resource; }

        protected:
            const BufferDesc    m_bufDesc;
            driver::Resource    m_resource;
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
        Buffer(const BufferDesc & _bufDesc, const core::string & _name, void * _initData = nullptr);
        ~Buffer();
    };
}