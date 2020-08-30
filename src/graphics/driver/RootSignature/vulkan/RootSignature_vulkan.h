#pragma once

#include "graphics/driver/RootSignature/RootSignature.h"

namespace vg::graphics::driver::vulkan
{
    class RootSignature : public base::RootSignature
    {
        using super = base::RootSignature;

    public:
        RootSignature(const RootSignatureDesc & _desc);
        ~RootSignature();
    };
}