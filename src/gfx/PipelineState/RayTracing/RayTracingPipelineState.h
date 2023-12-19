#pragma once

#include "core/Object/Object.h"
#include "RayTracingPipelineState_consts.h"
#include "RayTracingPipelineStateKey.h"

namespace vg::gfx
{
    namespace base
    {
        class RayTracingPipelineState : public core::Object
        {
        public:
            RayTracingPipelineState(const RayTracingPipelineStateKey & _rayTracingKey);
            ~RayTracingPipelineState();

        private:
            RayTracingPipelineStateKey m_rayTracingKey;
        };
    }
}

#include VG_GFXAPI_HEADER(RayTracingPipelineState)

namespace vg::gfx
{
    class RayTracingPipelineState : public VG_GFXAPI::RayTracingPipelineState
    {
        using super = VG_GFXAPI::RayTracingPipelineState;

    public:
        const char * getClassName() const final { return "RayTracingPipelineState"; }

        RayTracingPipelineState(const RayTracingPipelineStateKey & _rayTracingKey);
        ~RayTracingPipelineState();
    };
}