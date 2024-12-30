#include "RenderJob.h"

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    RenderJob::RenderJob(const core::string & _name, core::uint _index, core::IObject * _parent) :
        core::Job(fmt::sprintf("%s #%u", _name, _index), _parent),
        m_index(_index)
    {

    }

    //--------------------------------------------------------------------------------------
    void RenderJob::Run()
    {
        VG_PROFILE_GPU_CONTEXT(m_cmdList);

        FrameGraph * framegraph = VG_SAFE_STATIC_CAST(FrameGraph, getParent());
        for (uint i = 0; i < m_nodes.size(); ++i)
        {
            auto * node = m_nodes[i];
            framegraph->RenderNode(m_cmdList, node);
        }
    }

    //--------------------------------------------------------------------------------------
    void RenderJob::reset(gfx::CommandList * _cmdList)
    {
        m_nodes.clear();
        m_cmdList = _cmdList;
    }

    //--------------------------------------------------------------------------------------
    void RenderJob::add(const UserPassInfoNode * _nodeToRender)
    {
        m_nodes.push_back(_nodeToRender);
    }
}