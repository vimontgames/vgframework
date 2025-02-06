#pragma once

#include "core/Object/Object.h"
#include "ComputePipelineState_consts.h"
#include "ComputePipelineStateKey.h"

namespace vg::gfx
{
    namespace base
    {
        class ComputePipelineState : public core::Object
        {
        public:
            ComputePipelineState(const ComputePipelineStateKey & _computekey);
            ~ComputePipelineState();

        private:
            ComputePipelineStateKey m_computeKey;
        };
    }
}

#ifdef VG_DX12
#include "dx12/ComputePipelineState_dx12.h"
#elif defined(VG_VULKAN)
#include "vulkan/ComputePipelineState_vulkan.h"
#else
#error Undefined GFXAPI
#endif

namespace vg::gfx
{
    class ComputePipelineState : public VG_GFXAPI::ComputePipelineState
    {
        using super = VG_GFXAPI::ComputePipelineState;

    public:
        const char * GetClassName() const final { return "ComputePipelineState"; }

        static ComputePipelineState * createComputePipelineState(const ComputePipelineStateKey & _computeKey);

        ComputePipelineState(const ComputePipelineStateKey & _computeKey);
        ~ComputePipelineState();
    };
}