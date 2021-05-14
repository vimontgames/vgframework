#pragma once

#include "core/Object/Object.h"
#include "RootSignature_consts.h"

namespace vg::graphics::driver
{
    namespace base
    {
        class RootSignature : public core::ObjectPointer
        {
        public:
            RootSignature(const RootSignatureDesc & _desc);
            ~RootSignature();

            const RootSignatureDesc & getRootSignatureDesc() const { return m_desc; }

        private:
            const RootSignatureDesc m_desc;
        };
    }
}

#include VG_GFXAPI_HEADER(RootSignature)

namespace vg::graphics::driver
{
    class RootSignature : public VG_GFXAPI::RootSignature
    {
        using super = VG_GFXAPI::RootSignature;

    public:
        const char * getClassName() const final { return "RootSignature"; }

        RootSignature(const RootSignatureDesc & _desc);
        ~RootSignature();
    };
}