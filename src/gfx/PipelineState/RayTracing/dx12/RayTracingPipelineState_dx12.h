#pragma once

#include "gfx/PipelineState/RayTracing/RayTracingPipelineState.h"

namespace vg::gfx
{
    class RayTracingPipelineState;

    namespace dx12
    {
        class RayTracingPipelineState : public base::RayTracingPipelineState
        {
            using super = base::RayTracingPipelineState;

        public:
            RayTracingPipelineState(const RayTracingPipelineStateKey & _key);
            ~RayTracingPipelineState();

            ID3D12StateObject * getd3d12RayTracingPipelineState() const;
            
            static gfx::RayTracingPipelineState * createRayTracingPipelineState(const RayTracingPipelineStateKey & _rayTracingKey);

        private:
            void setd3d12RayTracingPipelineState(ID3D12StateObject * _d3d12RayTracingPipelineState);
            static bool created3d12RayTracingPipelineState(const RayTracingPipelineStateKey & _rayTracingKey, ID3D12StateObject *& _d3d12RayTracingPipelineState);

        private:
            ID3D12StateObject * m_d3d12RayTracingPipelineState = nullptr;
        };
    }
}