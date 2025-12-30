#include "ViewCullingJob.h"
#include "core/string/string.h"
#include "renderer/View/View.h"
#include "core/IBaseScene.h"
#include "core/GameObject/GameObject.h"
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
    ViewCullingJob::ViewCullingJob(const string & _name, IObject * _parent, ViewCullingJobOutput * const _output, SharedViewCullingJobOutput * const _sharedOutput) :
        Job(_name, _parent)
    {
        VG_ASSERT(_output);
        VG_ASSERT(_sharedOutput);

        m_result.m_output = _output;
        m_result.m_sharedOutput = _sharedOutput;
    }
    
    //--------------------------------------------------------------------------------------
    template <typename T> void cullInstanceList(const ViewCullingOptions _cullingOptions, const T & _instanceList, CullingResult & _result)
    {
        const auto instanceCount = _instanceList.size();
        for (uint i = 0; i < instanceCount; ++i)
        {
            GraphicInstance * instance = (GraphicInstance *)_instanceList[i];

            if (instance->isEnabledInHierarchy())
                instance->Cull(_cullingOptions, &_result);
        }
    }

    //--------------------------------------------------------------------------------------
    void ViewCullingJob::Run()
    {
        m_result.m_output->clear();

        ViewCullingOptions cullingOptions;
        cullingOptions.m_view = (View *)GetParent();
        
        gfx::ViewID viewID = cullingOptions.m_view->getViewID();

        if (Renderer::get()->IsFullscreen() && viewID.target == gfx::ViewTarget::Editor)
            return;

        if (viewID.target == gfx::ViewTarget::Game || viewID.target == gfx::ViewTarget::Editor)
        {
            if (!cullingOptions.m_view->IsRender())
                return;
        }
       
        string name = fmt::sprintf("Culling \"%s\" View #%u", asString(viewID.target), viewID.index);
        VG_PROFILE_CPU(name.c_str());
        //VG_DEBUGPRINT("\"%s\" running on \"%s\" (0x%08X)\n", name.c_str(), Kernel::getScheduler()->GetCurrentThreadName().c_str(), Kernel::getScheduler()->GetCurrentThreadID());

        const auto * world = cullingOptions.m_view->getWorld();
        
        if (nullptr != world)
        {
            for (uint iSceneType = 0; iSceneType < enumCount<BaseSceneType>(); iSceneType++)
            {
                BaseSceneType sceneType = (BaseSceneType)iSceneType;
                if (sceneType != BaseSceneType::Scene)
                    continue;

                const uint sceneCount = world->GetSceneCount(sceneType);
                for (uint jScene = 0; jScene < sceneCount; ++jScene)
                {
                    const IBaseScene * scene = world->GetScene(jScene, sceneType);
                    SceneRenderData * sceneRenderData = (SceneRenderData *)scene->GetSceneRenderData();
                    VG_ASSERT(sceneRenderData);

                    // Cull cameras
                    {
                        VG_PROFILE_CPU("Cameras");
                        cullInstanceList(cullingOptions, sceneRenderData->m_cameraInstances, m_result);
                    }

                    // Cull Lights
                    {
                        VG_PROFILE_CPU("Lights");
                        cullInstanceList(cullingOptions, sceneRenderData->m_lightInstances, m_result);
                    }

                    // Cull Particles
                    {
                        VG_PROFILE_CPU("Particles");
                        cullInstanceList(cullingOptions, sceneRenderData->m_particleSystemInstances, m_result);
                    }

                    // Cull Skinned Meshes
                    {
                        VG_PROFILE_CPU("Skinned");
                        cullInstanceList(cullingOptions, sceneRenderData->m_skinnedMeshInstances, m_result);
                    }

                    // Cull Static Meshes
                    {
                        VG_PROFILE_CPU("Meshes");
                        cullInstanceList(cullingOptions, sceneRenderData->m_staticMeshInstances, m_result);
                    }                 
                }
            }

            // Sort list that need it
            {
                VG_PROFILE_CPU("Sort");

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
}
