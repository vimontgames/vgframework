#pragma once

#include "core/Object/Object.h"
#include "RootSignature_consts.h"

namespace vg::gfx
{
    namespace base
    {
        class RootSignature : public core::Object
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

namespace vg::gfx
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