#include "ViewCullingJob.h"
#include "core/string/string.h"
#include "renderer/View/View.h"
#include "core/IBaseScene.h"
#include "core/GameObject/GameObject.h"
#include "renderer/IGraphicInstance.h"
#include "renderer/Instance/Mesh/MeshInstance.h"
#include "renderer/Options/RendererOptions.h"
#include "renderer/Renderer.h"

using namespace vg::core;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    void ViewCullingJobOutput::clear()
    {
        for (core::uint i = 0; i < core::enumCount<GraphicInstanceListType>(); ++i)
            m_instancesLists[i].clear();

        for (core::uint i = 0; i < core::enumCount<LightType>(); ++i)
            m_lightsLists[i].clear();

        m_cameras.clear();

        for (core::uint i = 0; i < m_additionalViews.size(); ++i)
            VG_SAFE_RELEASE(m_additionalViews[i]);
        m_additionalViews.clear();
    }

    //--------------------------------------------------------------------------------------
    ViewCullingJob::ViewCullingJob(const string & _name, IObject * _parent, ViewCullingJobOutput * const _output, SharedCullingJobOutput * const _sharedOutput) :
        Job(_name, _parent)
    {
        VG_ASSERT(_output);
        VG_ASSERT(_sharedOutput);

        m_result.m_output = _output;
        m_result.m_sharedOutput = _sharedOutput;
    }

    //--------------------------------------------------------------------------------------
    void ViewCullingJob::cullGameObjectRecur(const GameObject * _go, View * _view)
    {
        // Visible? Then add it to the list
        if (asBool(InstanceFlags::Enabled & _go->getInstanceFlags()))
        {
            auto & instances = _go->getGraphicInstances();
            const auto instanceCount = instances.size();
            for (uint i = 0; i < instanceCount; ++i)
            {
                GraphicInstance * instance = (GraphicInstance*)instances[i];

                if (asBool(InstanceFlags::Enabled & instance->getInstanceFlags()))
                    instance->Cull(&m_result, _view);
            }

            const auto & children = _go->GetChildren();
            const auto childCount = children.size();
            for (uint i = 0; i < childCount; ++i)
            {
                const GameObject * child = VG_SAFE_STATIC_CAST(GameObject, children[i]);
                cullGameObjectRecur(child, _view);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void ViewCullingJob::Run()
    {
        m_result.m_output->clear();

        View * view = (View *)GetParent();
        gfx::ViewID viewID = view->getViewID();

        if (Renderer::get()->IsFullscreen() && viewID.target == gfx::ViewTarget::Editor)
            return;

        if (viewID.target == gfx::ViewTarget::Game || viewID.target == gfx::ViewTarget::Editor)
        {
            if (!view->IsRender())
                return;
        }
       
        string name = fmt::sprintf("Culling %s #%u", asString(viewID.target), viewID.index);
        VG_PROFILE_CPU(name.c_str());
        //VG_DEBUGPRINT("\"%s\" running on \"%s\" (0x%08X)\n", name.c_str(), Kernel::getScheduler()->GetCurrentThreadName().c_str(), Kernel::getScheduler()->GetCurrentThreadID());

        const auto * world = view->getWorld();
        const auto & frustum = view->getCameraFrustum();
        
        if (nullptr != world)
        {
            const uint count = world->GetSceneCount(BaseSceneType::Scene);
            for (uint i = 0; i < count; ++i)
            {
                const auto * scene = world->GetScene(i, BaseSceneType::Scene);
                const auto * root = scene->GetRoot();
                VG_ASSERT("[Culling] Scene has no root node");
                if (root)
                    cullGameObjectRecur((GameObject *)root, view);
            }

            const float4x4 & viewMatrix = view->GetViewInvMatrix();

            for (uint i = 0; i < core::enumCount<GraphicInstanceListType>(); ++i)
            {
                const auto listType = (GraphicInstanceListType)i;
                auto & list = m_result.m_output->m_instancesLists[i].m_instances;

                if (list.size() > 1)
                {
                    switch (listType)
                    {
                        default:
                            VG_ASSERT_ENUM_NOT_IMPLEMENTED(listType);
                            break;

                        case GraphicInstanceListType::All:
                        case GraphicInstanceListType::Opaque:
                        case GraphicInstanceListType::AlphaTest:
                        case GraphicInstanceListType::Decal:
                            break;

                        case GraphicInstanceListType::Transparent:
                        {
                            VG_PROFILE_CPU("Back-to-front");

                            sort(list.begin(), list.end(), [=](GraphicInstance * const a, GraphicInstance * const b)
                            {
                                const float4x4 & matA = a->GetGlobalMatrix();
                                const float4 & viewPosA = mul(viewMatrix, matA[3]);
                            
                                const float4x4 & matB = b->GetGlobalMatrix();
                                const float4 & viewPosB = mul(viewMatrix, matB[3]);
                                
                                return (bool)(viewPosA.z < viewPosB.z);
                            }
                            );
                        }
                        break;

                        case GraphicInstanceListType::Outline:
                            break;
                    }
                }
            }
        }
    }
}
