#pragma once

#include "core/Object/Object.h"
#include "RootSignature_consts.h"

namespace vg::graphics::driver
{
    namespace base
    {
        class RootSignature : public core::Object
        {
        public:
            RootSignature(const RootSignatureDesc & _desc);
            ~RootSignature();

        private:
            const RootSignatureDesc m_desc;
        };
    }
}

#include VG_GRAPHICSAPI_HEADER(RootSignature)

namespace vg::graphics::driver
{
    class RootSignature : public VG_GRAPHICSAPI::RootSignature
    {
        using super = VG_GRAPHICSAPI::RootSignature;

    public:
        RootSignature(const RootSignatureDesc & _desc);
        ~RootSignature();
    };
}