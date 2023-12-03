#include "ViewCullingJob.h"
#include "renderer/View/View.h"
#include "core/Scene/Scene.h"
#include "core/GameObject/GameObject.h"
#include "renderer/IGraphicInstance.h"
#include "renderer/Instance/Mesh/MeshInstance.h"

using namespace vg::core;

namespace vg::renderer
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
        if (asBool(GameObject::Flags::Enabled & _go->getFlags()))
        {
            auto & instances = _go->getGraphicInstances();
            const auto instanceCount = instances.size();
            for (uint i = 0; i < instanceCount; ++i)
            {
                IGraphicInstance * instance = instances[i];

                if (asBool(core::Instance::Flags::Enabled & instance->getFlags()))
                {
                    bool visible = true;

                    if (visible)
                        dispatch(instance);
                }
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
    VG_INLINE void ViewCullingJob::dispatch(IGraphicInstance * _instance)
    {
        add(GraphicInstanceListType::All, _instance);

        if (1) // TODO
            add(GraphicInstanceListType::Opaque, _instance);

        if (0) // TODO
            add(GraphicInstanceListType::Transparent, _instance);

        if (_instance->IsSkinned())
        {
            VG_ASSERT(dynamic_cast<MeshInstance *>(_instance));
            MeshInstance * meshInstance = (MeshInstance *)_instance;
            if (meshInstance->setSkinFlag(MeshInstance::SkinFlags::SkinLOD0))
                m_output->m_skins.push_back(meshInstance);
        }
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void ViewCullingJob::add(GraphicInstanceListType _type, const IGraphicInstance * _instance)
    {
        m_output->m_instanceLists[asInteger(_type)].m_instances.push_back(_instance);
    }

    //--------------------------------------------------------------------------------------
    void ViewCullingJob::run()
    {
        m_output->clear();

        View * view = (View *)getParent();
        gfx::ViewID viewID = view->getViewID();
        string name = getName() + " " + asString(viewID.target) + " " + to_string(viewID.index);
        VG_PROFILE_CPU(name.c_str());
        //VG_DEBUGPRINT("\"%s\" running on \"%s\" (0x%08X)\n", name.c_str(), Kernel::getScheduler()->GetCurrentThreadName().c_str(), Kernel::getScheduler()->GetCurrentThreadID());

        // "Universe" is a shitty name, I have to find something better like e.g. "SceneList" and cull by scene using separate jobs
        const auto * uni = view->getUniverse();
        const uint count = uni->GetSceneCount();
        for (uint i = 0; i < count; ++i)
        {
            const auto * scene = (Scene*)uni->GetScene(i);
            const auto * root = scene->getRoot();
            cullGameObjectRecur(root);
        }
    }
}
