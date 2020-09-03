#pragma once

#include "core/Container/HandleTable.h"
#include "RootSignatureDesc.h"

namespace vg::graphics::driver
{
    class RootSignature;

    using RootSignatureTable = core::HandleTable<RootSignatureDesc, RootSignature, core::u8>;
    using RootSignatureHandle = RootSignatureTable::Handle;
}