#pragma once

#include "core/Scheduler/Job.h"
#include "GraphicInstanceList.h"
#include "core/Container/AtomicVector.h"

namespace vg::core
{
    class GameObject;
}

namespace vg::renderer
{
    class View;
    class IGraphicInstance;
    class MeshInstance;

    struct Frustum;
 
    struct ViewCullingJobOutput
    {
        void clear()
        {
            for (core::uint i = 0; i < core::enumCount<GraphicInstanceListType>(); ++i)
                m_instanceLists[i].clear();
        }

        void add(GraphicInstanceListType _type, const IGraphicInstance * _instance)
        {
            m_instanceLists[core::asInteger(_type)].m_instances.push_back(_instance);
        }

        GraphicInstanceList m_instanceLists[core::enumCount<GraphicInstanceListType>()];
    };

    struct SharedCullingJobOutput
    {
        SharedCullingJobOutput() :
            m_skins(1024)
        {

        }

        void clear()
        {
            m_skins.clear();
        }

        core::atomicvector<MeshInstance *>  m_skins;
    };

    struct CullingResult
    {
        ViewCullingJobOutput *      m_output        = nullptr;
        SharedCullingJobOutput *    m_sharedOutput  = nullptr;
    };

    class ViewCullingJob : public core::Job
    {
    public:
        const char * getClassName() const final { return "ViewCullingJob"; }

        ViewCullingJob(const core::string & _name, core::IObject * _parent, ViewCullingJobOutput * const _output, SharedCullingJobOutput * const _sharedOutput);
        void run() override;

    protected:
        void cullGameObjectRecur(const core::GameObject * _go, const Frustum & _frustum);

    private:
        //VG_INLINE void dispatch(IGraphicInstance * _instance);
        VG_INLINE void add(GraphicInstanceListType _type, const IGraphicInstance * _instance);

    private:
        CullingResult m_result;
    };
}