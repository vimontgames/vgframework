#pragma once

#include "core\Scheduler\Job.h"

namespace vg::core
{
    class GameObject;
}

namespace vg::renderer
{
    class View;
    class IGraphicInstance;

    struct ViewCullingJobOutput
    {
        core::vector<const IGraphicInstance *> m_visibleGraphicInstances;
    };

    class ViewCullingJob : public core::Job
    {
    public:
        const char * getClassName() const final { return "ViewCullingJob"; }

        ViewCullingJob(const core::string & _name, core::IObject * _parent, ViewCullingJobOutput * const _output);
        void run() override;

    protected:
        void cullGameObjectRecur(const core::GameObject * _go);

    private:
       ViewCullingJobOutput * const m_output = nullptr;
    };
}