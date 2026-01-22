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
    ViewCullingFinalJob::ViewCullingFinalJob(const string & _name, IObject * _parent) :
        Job(_name, _parent)
    {
       
    }

    //--------------------------------------------------------------------------------------
    void ViewCullingFinalJob::setup(ViewCullingJobOutput * const _output, SharedViewCullingJobOutput * const _sharedOutput, const core::vector<ViewCullingJobOutput> * _splitResults)
    {
        VG_ASSERT(_output);
        m_result.m_output = _output;

        VG_ASSERT(_sharedOutput);
        m_result.m_sharedOutput = _sharedOutput;

        m_splitResults = _splitResults;
    }
    
    //--------------------------------------------------------------------------------------
    template <typename T> void cullInstanceList(const char * _label, const ViewCullingOptions _cullingOptions, const T & _instanceList, CullingResult & _result, core::uint _start, core::uint _count, core::uint & _arrayStart)
    {
        const uint instanceCount = (uint)_instanceList.size();

        const uint globalBegin = _start;
        const uint globalEnd = _start + _count;

        const uint arrayBegin = _arrayStart;
        const uint arrayEnd = _arrayStart + instanceCount;

        //  global intersection
        const uint begin = core::max(globalBegin, arrayBegin);
        const uint end = core::min(globalEnd, arrayEnd);

        if (begin < end)
        {
            VG_PROFILE_CPU(_label);

            const uint localBegin = begin - arrayBegin;
            const uint localEnd = end - arrayBegin;

            for (uint i = localBegin; i < localEnd; ++i)
            {
                GraphicInstance * instance = (GraphicInstance *)_instanceList[i];

                if (instance->isEnabledInHierarchy())
                    instance->Cull(_cullingOptions, &_result);
            }
        }

        _arrayStart += instanceCount;
    }

    //--------------------------------------------------------------------------------------
    void CullSceneObjects(const IWorld * world, const ViewCullingOptions _cullingOptions, CullingResult & _result, core::uint _start = 0, core::uint _count = -1)
    {
        uint counter = 0;

        // Cull scene objects
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

                uint arrayStart = 0;

                // Cull cameras
                cullInstanceList("Cameras", _cullingOptions, sceneRenderData->m_cameraInstances, _result, _start, _count, arrayStart);

                // Cull Lights
                cullInstanceList("Lights", _cullingOptions, sceneRenderData->m_lightInstances, _result, _start, _count, arrayStart);

                // Cull Particles
                cullInstanceList("Particles", _cullingOptions, sceneRenderData->m_particleSystemInstances, _result, _start, _count, arrayStart);

                // Cull Skinned Meshes
                cullInstanceList("Skinned", _cullingOptions, sceneRenderData->m_skinnedMeshInstances, _result, _start, _count, arrayStart);


                // Cull Static Meshes
                cullInstanceList("Meshes", _cullingOptions, sceneRenderData->m_staticMeshInstances, _result, _start, _count, arrayStart);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void ViewCullingFinalJob::Run()
    {
        m_result.m_output->clear();

        ViewCullingOptions cullingOptions;
        cullingOptions.m_view = (View *)GetParent();
        
        gfx::ViewID viewID = cullingOptions.m_view->getViewID();

        if (Renderer::get()->IsGameMode() && viewID.target == gfx::ViewTarget::Editor)
            return;

        if (viewID.target == gfx::ViewTarget::Game || viewID.target == gfx::ViewTarget::Editor)
        {
            if (!cullingOptions.m_view->IsRender())
                return;
        }
       
        VG_PROFILE_CPU("Culling");
        {
            //VG_DEBUGPRINT("\"%s\" running on \"%s\" (0x%08X)\n", name.c_str(), Kernel::getScheduler()->GetCurrentThreadName().c_str(), Kernel::getScheduler()->GetCurrentThreadID());
            string name;
            if (auto * parentView = VG_SAFE_STATIC_CAST(View, cullingOptions.m_view->getParent()))
                name = fmt::sprintf("%s #%u from %s #%u", asString(viewID.target), viewID.index, asString(parentView->getViewID().target), parentView->getViewID().index);
            else
                name = fmt::sprintf("%s #%u", asString(viewID.target), viewID.index);

            VG_PROFILE_CPU(name.c_str());

            const auto * world = cullingOptions.m_view->getWorld();

            if (nullptr != world)
            {
                if (RendererOptions::get()->isSplitCullingJobsEnabled())
                {
                    VG_PROFILE_CPU("Merge");

                    // Merge results from split jobs (TODO: reserve?)
                    for (uint i = 0; i < m_splitResults->size(); ++i)
                    {
                        const ViewCullingJobOutput & splitResults = (*m_splitResults)[i];

                        m_result.m_output->m_cameras.insert(m_result.m_output->m_cameras.end(), splitResults.m_cameras.begin(), splitResults.m_cameras.end());

                        for (uint j = 0; j < core::enumCount<LightType>(); ++j)
                            m_result.m_output->m_lightsLists[j].m_instances.insert(m_result.m_output->m_lightsLists[j].m_instances.end(), splitResults.m_lightsLists[j].m_instances.begin(), splitResults.m_lightsLists[j].m_instances.end());
                        
                        for (uint j = 0; j < core::enumCount<GraphicInstanceListType>(); ++j)
                            m_result.m_output->m_instancesLists[j].m_instances.insert(m_result.m_output->m_instancesLists[j].m_instances.end(), splitResults.m_instancesLists[j].m_instances.begin(), splitResults.m_instancesLists[j].m_instances.end());
                        
                        //m_result.m_output->m_additionalViews.insert(m_result.m_output->m_additionalViews.end(), splitResults.m_additionalViews.begin(), splitResults.m_additionalViews.end());
                        //for (uint j = 0; j < m_result.m_output->m_additionalViews.size(); ++j)
                        //    VG_SAFE_INCREASE_REFCOUNT(m_result.m_output->m_additionalViews[j]);
                    }
                }
                else
                {
                    CullSceneObjects(world, cullingOptions, m_result);
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

                                // The following lists do not need sort
                                case GraphicInstanceListType::All:
                                case GraphicInstanceListType::Decal:
                                case GraphicInstanceListType::Particle:
                                case GraphicInstanceListType::Outline:
                                    break;

                                // Opaques should be sorted front-to-back to improve ZCull, but sorting 
                                // opaques front-to-back is not worth it with the current implementation 
                                // and only saved -0.04 ms on the GPU bust cost +0.44 ms on the CPU. 
                                case GraphicInstanceListType::Opaque:
                                case GraphicInstanceListType::AlphaTest:
                                {
                                    //VG_PROFILE_CPU("front-to-back");
                                    //
                                    //sort(list.begin(), list.end(), [=](GraphicInstance * const a, GraphicInstance * const b)
                                    //    {
                                    //        const float4x4 & matA = a->GetGlobalMatrix();
                                    //        const float4 & viewPosA = mul(viewMatrix, matA[3]);
                                    //
                                    //        const float4x4 & matB = b->GetGlobalMatrix();
                                    //        const float4 & viewPosB = mul(viewMatrix, matB[3]);
                                    //
                                    //        return (bool)(viewPosA.z > viewPosB.z);
                                    //    }
                                    //);
                                }
                                break;

                                // Transparent should be sorted back-to-front for alpha blending
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
                            }
                        }
                    }
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    // N 'ViewCullingSplitJob' jobs per view, each of them working on a range of the registered visuals
    //--------------------------------------------------------------------------------------
    ViewCullingSplitJob::ViewCullingSplitJob(const core::string & _name, core::IObject * _parent, ViewCullingJobOutput * const _output, SharedViewCullingJobOutput * const _sharedOutput, core::uint _index) :
        Job(_name, _parent),
        m_index(_index)
    {
        VG_ASSERT(_output);
        VG_ASSERT(_sharedOutput);

        m_result.m_output = _output;
        m_result.m_sharedOutput = _sharedOutput;
    }

    //--------------------------------------------------------------------------------------
    void ViewCullingSplitJob::setup(core::uint _first, core::uint _count)
    {
        m_first = _first;
        m_count = _count;
    }

    //--------------------------------------------------------------------------------------
    void ViewCullingSplitJob::Run()
    {
        m_result.m_output->clear();

        ViewCullingOptions cullingOptions;
        cullingOptions.m_view = (View *)GetParent();

        gfx::ViewID viewID = cullingOptions.m_view->getViewID();
        const auto * world = cullingOptions.m_view->getWorld();

        VG_PROFILE_CPU("Culling");
        {
            string name;
            if (auto * parentView = VG_SAFE_STATIC_CAST(View, cullingOptions.m_view->getParent()))
                name = fmt::sprintf("%s #%u from %s #%u", asString(viewID.target), viewID.index, asString(parentView->getViewID().target), parentView->getViewID().index);
            else
                name = fmt::sprintf("%s #%u", asString(viewID.target), viewID.index);

            VG_PROFILE_CPU(name.c_str());
            CullSceneObjects(world, cullingOptions, m_result, m_first, m_count);
        }
    }
}
