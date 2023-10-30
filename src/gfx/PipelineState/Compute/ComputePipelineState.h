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

#include VG_GFXAPI_HEADER(ComputePipelineState)

namespace vg::gfx
{
    class ComputePipelineState : public VG_GFXAPI::ComputePipelineState
    {
        using super = VG_GFXAPI::ComputePipelineState;

    public:
        const char * getClassName() const final { return "ComputePipelineState"; }

        ComputePipelineState(const ComputePipelineStateKey & _computeKey);
        ~ComputePipelineState();
    };
}