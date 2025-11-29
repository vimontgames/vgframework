#include "ViewCullingJob.h"
#include "core/string/string.h"
#include "renderer/View/View.h"
#include "core/IBaseScene.h"
#include "core/GameObject/GameObject.h"
#include "renderer/IGraphicInstance.h"
#include "renderer/Instance/Mesh/MeshInstance.h"
#include "renderer/Options/RendererOptions.h"
#include "renderer/Renderer.h"
#include "renderer/RayTracing/RayTracingManager.h"

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
    void ViewCullingJob::cullGameObjectRecur(const CullingOptions & _cullingOptions, const core::GameObject * _gameobject)
    {
        // Visible? Then add it to the list
        if (asBool(InstanceFlags::Enabled & _gameobject->getInstanceFlags()))
        {
            auto & instances = _gameobject->getGraphicInstances();
            const auto instanceCount = instances.size();
            for (uint i = 0; i < instanceCount; ++i)
            {
                GraphicInstance * instance = (GraphicInstance*)instances[i];

                if (asBool(InstanceFlags::Enabled & instance->getInstanceFlags()))
                    instance->Cull(_cullingOptions, &m_result);
            }

            const auto & children = _gameobject->GetChildren();
            const auto childCount = children.size();
            for (uint i = 0; i < childCount; ++i)
            {
                const GameObject * child = VG_SAFE_STATIC_CAST(GameObject, children[i]);
                cullGameObjectRecur(_cullingOptions, child);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void ViewCullingJob::Run()
    {
        m_result.m_output->clear();

        CullingOptions cullingOptions;
        cullingOptions.m_view = (View *)GetParent();
        cullingOptions.m_raytracing = RendererOptions::get()->isRayTracingEnabled();
        
        gfx::ViewID viewID = cullingOptions.m_view->getViewID();

        if (Renderer::get()->IsFullscreen() && viewID.target == gfx::ViewTarget::Editor)
            return;

        if (viewID.target == gfx::ViewTarget::Game || viewID.target == gfx::ViewTarget::Editor)
        {
            if (!cullingOptions.m_view->IsRender())
                return;
        }
       
        string name = fmt::sprintf("Culling %s #%u", asString(viewID.target), viewID.index);
        VG_PROFILE_CPU(name.c_str());
        //VG_DEBUGPRINT("\"%s\" running on \"%s\" (0x%08X)\n", name.c_str(), Kernel::getScheduler()->GetCurrentThreadName().c_str(), Kernel::getScheduler()->GetCurrentThreadID());

        const auto * world = cullingOptions.m_view->getWorld();
        
        if (nullptr != world)
        {
            const uint count = world->GetSceneCount(BaseSceneType::Scene);
            for (uint i = 0; i < count; ++i)
            {
                const auto * scene = world->GetScene(i, BaseSceneType::Scene);
                const auto * root = (GameObject *)scene->GetRoot();
                VG_ASSERT("[Culling] Scene has no root node");
                if (root)
                    cullGameObjectRecur(cullingOptions, root);
            }

            const float4x4 & viewMatrix = cullingOptions.m_view->GetViewInvMatrix();

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
                        case GraphicInstanceListType::Particle:
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
