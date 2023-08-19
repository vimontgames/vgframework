#include "renderer/Precomp.h"
#include "RenderPass.h"

#include "core/Math/Math.h"

#include "gfx/Device/Device.h"
#include "gfx/CommandList/CommandList.h"
#include "gfx/Resource/Texture.h"
#include "gfx/Resource/Buffer.h"
#include "gfx/Resource/Resource_consts.h"
#include "gfx/PipelineState/DepthStencilState.h"
#include "gfx/PipelineState/RasterizerState.h"
#include "gfx/PipelineState/BlendState.h"
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
#include "Render2D/FinalPostProcess/FinalPostProcessPass.hpp"

#include "RenderObjects/RenderObjectsPass.hpp"
#include "RenderObjects/Forward/ForwardPass.hpp"

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