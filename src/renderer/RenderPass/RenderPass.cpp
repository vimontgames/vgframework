#include "renderer/Precomp.h"
#include "RenderPass.h"

#include "gfx/Device/Device.h"
#include "gfx/CommandList/CommandList.h"
#include "gfx/Resource/Texture.h"
#include "gfx/Resource/Buffer.h"
#include "gfx/Resource/Resource_consts.h"
#include "gfx/PipelineState/Graphic/DepthStencilState.h"
#include "gfx/PipelineState/Graphic/RasterizerState.h"
#include "gfx/PipelineState/Graphic/BlendState.h"
#include "gfx/RootSignature/RootSignature.h"
#include "gfx/Shader/ShaderManager.h"
#include "gfx/Shader/Shader.h"
#include "gfx/BindlessTable/BindlessTable.h"
#include "gfx/Profiler/Profiler.h"

#include "renderer/Renderer.h"

using namespace vg::core;
using namespace vg::gfx;

#include "Render2D/Render2DPass.hpp"
#include "Render2D/Background/BackgroundPass.hpp"
#include "Render2D/FinalBlit/FinalBlitPass.hpp"

#include "RenderObjects/RenderObjectsPass.hpp"
#include "RenderObjects/DepthOnly/DepthOnlyPass.hpp"
#include "RenderObjects/Forward/ForwardPass.hpp"
#include "RenderObjects/Editor/EditorPass.hpp"

#include "Compute/ComputePass.hpp"
#include "Compute/ComputePostProcess/ComputePostProcessPass.hpp"
#include "Compute/ComputeSkinning/ComputeSkinningPass.hpp"

#include "Update/UpdatePass.hpp"
#include "Update/ViewConstants/ViewConstantsUpdatePass.hpp"
#include "Update/TLAS/TLASUpdatePass.hpp"
#include "Update/BLAS/BLASUpdatePass.hpp"



namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    RenderPass::RenderPass(const core::string & _name) :
        gfx::UserPass(_name)
    {

    }

    //--------------------------------------------------------------------------------------
    RenderPass::~RenderPass()
    {

    }
}