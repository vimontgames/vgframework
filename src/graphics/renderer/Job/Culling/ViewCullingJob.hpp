#include "ViewCullingJob.h"
#include "graphics/renderer/View/View.h"
#include "core/Scene/Scene.h"
#include "core/GameObject/GameObject.h"
#include "graphics/renderer/IGraphicInstance.h"

using namespace vg::core;

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    ViewCullingJob::ViewCullingJob(const string & _name, IObject * _parent, ViewCullingJobOutput * const _output) :
        Job(_name, _parent),
        m_output(_output)
    {

    }

    //--------------------------------------------------------------------------------------
    void ViewCullingJob::cullGameObjectRecur(const GameObject * _go)
    {
        // Visible? Then add it to the list
        if (GameObject::Flags::Enabled & _go->getFlags())
        {
            auto & instances = _go->GetGraphicInstances();
            const auto instanceCount = instances.size();
            for (uint i = 0; i < instanceCount; ++i)
            {
                IGraphicInstance * instance = instances[i];

                if (0 != (core::Instance::Flags::Enabled & instance->getFlags()))
                    m_output->m_visibleGraphicInstances.push_back(instance);
            }

            const auto & children = _go->getChildren();
            const auto childCount = children.size();
            for (uint i = 0; i < childCount; ++i)
            {
                const GameObject * child = children[i];
                cullGameObjectRecur(child);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void ViewCullingJob::run()
    {
        m_output->m_visibleGraphicInstances.clear();

        View * view = (View *)getParent();
        driver::ViewID viewID = view->getViewID();
        string name = getName() + " " + asString(viewID.type) + " " + to_string(viewID.index);
        VG_PROFILE_CPU(name.c_str());
        //VG_DEBUGPRINT("\"%s\" running on \"%s\" (0x%08X)\n", name.c_str(), Kernel::getScheduler()->GetCurrentThreadName().c_str(), Kernel::getScheduler()->GetCurrentThreadID());

        // "Universe" is a shitty name, I have to find something better like e.g. "SceneList" and cull by scene using separate jobs
        const auto * uni = view->getUniverse();
        const uint count = uni->getSceneCount();
        for (uint i = 0; i < count; ++i)
        {
            const auto * scene = (Scene*)uni->getScene(i);
            const auto * root = scene->getRoot();
            cullGameObjectRecur(root);
        }
    }
}
