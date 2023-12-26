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
        Job(_name, _parent),
        m_output(_output),
        m_sharedOutput(_sharedOutput)
    {
        VG_ASSERT(_output);
        VG_ASSERT(_sharedOutput);
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
                m_sharedOutput->m_skins.push_atomic(meshInstance);
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

        const auto * world = view->getWorld();
        if (nullptr != world)
        {
            const uint count = world->GetSceneCount();
            for (uint i = 0; i < count; ++i)
            {
                const auto * scene = world->GetScene(i);
                const auto * root = scene->GetRoot();
                VG_ASSERT("[Culling] Scene has no root node");
                if (root)
                    cullGameObjectRecur((GameObject *)root);
            }
        }
    }
}
