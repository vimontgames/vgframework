#include "engine/Precomp.h"
#include "AttachToNodeComponent.h"
#include "core/GameObject/GameObject.h"
#include "editor/Editor_Consts.h"
#include "engine/Component/Mesh/MeshComponent.h"
#include "renderer/IMeshInstance.h"
#include "renderer/ISkeleton.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(AttachToNodeComponent, "AttachToNode", "Animation", "Attach object to an animation node", editor::style::icon::Link, 0);

    //--------------------------------------------------------------------------------------
    bool AttachToNodeComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerOptionalProperty(AttachToNodeComponent, m_useTarget, m_target, "Target");
        setPropertyDescription(AttachToNodeComponent, m_target, "Use to specify another GameObject to look for skeleton. Otherwise the first skeleton found in parents will be used.");

        registerProperty(AttachToNodeComponent, m_boneName, "Bone");
        setPropertyDescription(AttachToNodeComponent, m_boneName, "The bone to follow. Bone names can be found under \"MeshComponent>Skeleton>Nodes\".");

        return true;
    }

    //--------------------------------------------------------------------------------------
    AttachToNodeComponent::AttachToNodeComponent(const core::string & _name, IObject * _parent) :
        super(_name, _parent)
    {
        clearCache();
    }

    //--------------------------------------------------------------------------------------
    AttachToNodeComponent::~AttachToNodeComponent()
    {

    }



    //--------------------------------------------------------------------------------------
    void AttachToNodeComponent::OnLoad()
    {
        super::OnLoad();
        updateCache();
    }

    //--------------------------------------------------------------------------------------
    void AttachToNodeComponent::OnEnable()
    {
        super::OnEnable();
        updateCache();
    }

    //--------------------------------------------------------------------------------------
    void AttachToNodeComponent::OnDisable()
    {
        
        super::OnDisable();
    }  

    //--------------------------------------------------------------------------------------
    void AttachToNodeComponent::Update(const Context & _context)
    {
        // TODO: this might be slow if the node is never found but need it because MeshInstance might not be immediately available
        if (!m_skeleton)
            updateCache();

        if (m_skeleton)
        {
            if (m_nodeIndex < (int)m_skeleton->GetNodeCount())
            {
                float4x4 mWorld = transpose(m_skeleton->GetNodeWorldMatrix(m_nodeIndex));
                getGameObject()->SetLocalMatrix(mWorld); 
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void AttachToNodeComponent::SetPropertyValue(const IProperty & _prop, void * _previousValue, void * _newValue)
    {
        super::SetPropertyValue(_prop, _previousValue, _newValue);

        if (&m_boneName == _previousValue)
            updateCache();
    }

    //--------------------------------------------------------------------------------------
    bool AttachToNodeComponent::updateCache()
    {
        clearCache();

        const GameObject * gameObject = nullptr;
        const renderer::ISkeleton * skeleton = nullptr;

        if (m_useTarget)
        {
            if (const GameObject * go = m_target.get<GameObject>())
            {
                if (const MeshComponent * mc = go->GetComponentT<MeshComponent>())
                {
                    if (const renderer::IMeshInstance * mi = mc->getMeshInstance())
                    {
                        if (const renderer::ISkeleton * sk = mi->GetSkeleton())
                            skeleton = sk;
                    }
                }
            }
        }
        else
        {
            if (const GameObject * go = getGameObject())
            {
                if (const MeshComponent * mc = go->GetComponentInParentsT<MeshComponent>())
                {
                    if (const renderer::IMeshInstance * mi = mc->getMeshInstance())
                    {
                        if (const renderer::ISkeleton * sk = mi->GetSkeleton())
                            skeleton = sk;
                    }
                }
            }
        }

        if (nullptr != skeleton)
        {
            m_skeleton = skeleton;
            m_nodeIndex = skeleton->FindNodeIndex(m_boneName);
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    void AttachToNodeComponent::clearCache()
    {
        m_skeleton = nullptr;
        m_nodeIndex = -1;
    }
}
