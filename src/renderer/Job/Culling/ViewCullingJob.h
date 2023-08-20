#pragma once

#include "core/Scheduler/Job.h"
#include "GraphicInstanceList.h"

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
        GraphicInstanceList m_instanceLists[core::enumCount<GraphicInstanceListType>()];

        void clear()
        {
            for (core::uint i = 0; i < core::enumCount<GraphicInstanceListType>(); ++i)
                m_instanceLists[i].clear();
        }
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
        VG_INLINE void dispatch(const IGraphicInstance * _instance);
        VG_INLINE void add(GraphicInstanceListType _type, const IGraphicInstance * _instance);

    private:
       ViewCullingJobOutput * const m_output = nullptr;
    };
}