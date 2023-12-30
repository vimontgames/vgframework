#include "ViewCullingJob.h"
#include "renderer/View/View.h"
#include "core/IScene.h"
#include "core/GameObject/GameObject.h"
#include "renderer/IGraphicInstance.h"
#include "renderer/Instance/Mesh/MeshInstance.h"
#include "renderer/Options/RendererOptions.h"

using namespace vg::core;

namespace vg::renderer
{
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
    void ViewCullingJob::cullGameObjectRecur(const GameObject * _go, const Frustum & _frustum)
    {
        // Visible? Then add it to the list
        if (asBool(GameObject::Flags::Enabled & _go->getFlags()))
        {
            auto & instances = _go->getGraphicInstances();
            const auto instanceCount = instances.size();
            for (uint i = 0; i < instanceCount; ++i)
            {
                GraphicInstance * instance = (GraphicInstance*)instances[i];

                if (asBool(core::Instance::Flags::Enabled & instance->getFlags()))
                    instance->Cull(_frustum, &m_result);
            }

            const auto & children = _go->getChildren();
            const auto childCount = children.size();
            for (uint i = 0; i < childCount; ++i)
            {
                const GameObject * child = children[i];
                cullGameObjectRecur(child, _frustum);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void ViewCullingJob::run()
    {
        m_result.m_output->clear();

        View * view = (View *)getParent();
        gfx::ViewID viewID = view->getViewID();
        string name = getName() + " " + asString(viewID.target) + " " + to_string(viewID.index);
        VG_PROFILE_CPU(name.c_str());
        //VG_DEBUGPRINT("\"%s\" running on \"%s\" (0x%08X)\n", name.c_str(), Kernel::getScheduler()->GetCurrentThreadName().c_str(), Kernel::getScheduler()->GetCurrentThreadID());

        const auto * world = view->getWorld();
        const auto & frustum = view->getCameraFrustum();
        
        if (nullptr != world)
        {
            const auto & frustum = view->getCameraFrustum();

            const uint count = world->GetSceneCount();
            for (uint i = 0; i < count; ++i)
            {
                const auto * scene = world->GetScene(i);
                const auto * root = scene->GetRoot();
                VG_ASSERT("[Culling] Scene has no root node");
                if (root)
                    cullGameObjectRecur((GameObject *)root, frustum);
            }
        }
    }
}
