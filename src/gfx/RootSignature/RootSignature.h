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

#ifdef VG_DX12
#include "dx12/RootSignature_dx12.h"
#elif defined(VG_VULKAN)
#include "vulkan/RootSignature_vulkan.h"
#else
#error Undefined GFXAPI
#endif

namespace vg::gfx
{
    class RootSignature : public VG_GFXAPI::RootSignature
    {
        using super = VG_GFXAPI::RootSignature;

    public:
        const char * GetClassName() const final { return "RootSignature"; }

        RootSignature(const RootSignatureDesc & _desc);
        ~RootSignature();
    };
}