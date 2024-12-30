#pragma once

#include "core/Scheduler/Job.h"

namespace vg::gfx
{
    class CommandList;
    struct UserPassInfoNode;

    class RenderJob final : public core::Job
    {
        public:
            const char * GetClassName() const final { return "RenderJob"; }

            RenderJob(const core::string & _name, core::uint _index, core::IObject * _parent);
            void Run() final override;

            void reset(gfx::CommandList * _cmdList);
            void add(const UserPassInfoNode * _nodeToRender);

        private:
            core::uint                              m_index = core::uint(-1);
            gfx::CommandList *                      m_cmdList = nullptr;
            core::vector<const UserPassInfoNode *>  m_nodes;
    };
}