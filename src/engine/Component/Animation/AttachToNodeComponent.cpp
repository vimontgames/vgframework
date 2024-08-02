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

        registerProperty(AttachToNodeComponent, m_NodeName, "Node");

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
    void AttachToNodeComponent::Update(float _dt)
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

        if (&m_NodeName == _previousValue)
            updateCache();
    }

    //--------------------------------------------------------------------------------------
    bool AttachToNodeComponent::updateCache()
    {
        clearCache();

        if (const GameObject * go = getGameObject())
        {
            if (const MeshComponent * mc = go->GetComponentInParents<MeshComponent>())
            {
                if (const renderer::IMeshInstance * mi = mc->getMeshInstance())
                {
                    if (const renderer::ISkeleton * sk = mi->GetSkeleton())
                    {
                        m_skeleton = sk;
                        m_nodeIndex = sk->FindNodeIndex(m_NodeName);
                        return true;
                    }
                }
            }
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
