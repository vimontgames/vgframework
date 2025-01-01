#include "gfx/Precomp.h"
#include "gfx/Device/Device.h"
#include "gfx/Shader/ShaderManager.h"
#include "gfx/Shader/Shader.h"
#include "gfx/Shader/HLSLDesc.h"
#include "gfx/RootSignature/RootSignature.h"
#include "gfx/Resource/Texture.h"
#include "GraphicPipelineState.h"
#include "DepthStencilState.h"
#include "RasterizerState.h"
#include "BlendState.h"
#include "SamplerState.h"

using namespace vg::core;

#include "GraphicPipelineStateKey.hpp"
#include "DepthStencilState.hpp"
#include "RasterizerState.hpp"
#include "BlendState.hpp"
#include "SamplerState.hpp"

namespace vg::gfx
{
    namespace base
    {
        //--------------------------------------------------------------------------------------
        GraphicPipelineState::GraphicPipelineState(const GraphicPipelineStateKey & _key) :
            m_key(_key)
        {

        }

        //--------------------------------------------------------------------------------------
        GraphicPipelineState::~GraphicPipelineState()
        {

        }

    }
}

#include VG_GFXAPI_IMPL(GraphicPipelineState)

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    GraphicPipelineState::GraphicPipelineState(const GraphicPipelineStateKey & _key) :
        super(_key)
    {

    }

    //--------------------------------------------------------------------------------------
    GraphicPipelineState::~GraphicPipelineState()
    {

    }

    //--------------------------------------------------------------------------------------
    GraphicPipelineState * GraphicPipelineState::createGraphicPipelineState(const GraphicPipelineStateKey & _key)
    {
        VG_PROFILE_CPU("CreateGraphicsPSO");
        _key.verify();
        return super::createGraphicPipelineState(_key);
    }
}