#include "ComputePass.h"

#include "Compute/ComputePostProcess/ComputePostProcessPass.hpp"
#include "Compute/ComputeSkinning/ComputeSkinningPass.hpp"
#include "Compute/ComputeDeferredLighting/ComputeDeferredLightingPass.hpp"
#include "Compute/ComputeSpecularBRDF/ComputeSpecularBRDFPass.hpp"
#include "Compute/ComputeIBLCubemaps/ComputeIBLCubemapsPass.hpp"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    ComputePass::ComputePass(const core::string & _name) :
        RenderPass(_name)
    {
        setUserPassType(RenderPassType::Compute);
    }

    //--------------------------------------------------------------------------------------
    ComputePass::~ComputePass()
    {

    }
}