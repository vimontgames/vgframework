namespace vg::graphics::driver
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

namespace vg::graphics::driver
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
}