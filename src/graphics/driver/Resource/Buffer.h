#pragma once

#include "Buffer_consts.h"
#include "core/Object/Object.h"
#include "graphics/driver/Resource/Resource.h"
#include "graphics/driver/BindlessTable/BindlessTable_consts.h"

namespace vg::graphics::driver
{
    namespace base
    {
        class Buffer : public core::Object
        {
        public:
            Buffer(const BufferDesc & _bufDesc, const core::string & _name, void * _initData);
            ~Buffer();

            const BufferDesc & getBufDesc() const { return m_bufDesc; }

            const driver::Resource & getResource() const { return m_resource; }
            driver::Resource & getResource() { return m_resource; }
            const driver::BindlessBufferSrvHandle getBindlessSRVHandle() const { return m_bindlessSRVHandle; }

        protected:
            const BufferDesc                m_bufDesc;
            driver::Resource                m_resource;
            driver::BindlessBufferSrvHandle m_bindlessSRVHandle;
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
        Buffer(const BufferDesc & _bufDesc, const core::string & _name, void * _initData = nullptr, ReservedSlot _reservedSlot = ReservedSlot::None);
        ~Buffer();
    };
}