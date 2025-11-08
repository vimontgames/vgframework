#include "ImGuiObjectPropertyContext.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    // Warning: This *WILL* change _object and _prop in case of override
    //--------------------------------------------------------------------------------------
    PropertyContext::PropertyContext(core::IObject *& _object, const core::IProperty *& _prop)
    {
        m_gameobject = _object->GetParentGameObject();
        m_originalObject = _object;
        m_originalProp = _prop;
        m_isPrefabInstance = false;     
        m_isPrefabOverride = false;     
        m_canPrefabOverride = false;    
        m_prefab = nullptr;             
        m_propOverride = nullptr;       

        if (m_gameobject && _object->GetUID(false))
        {
            // Cannot override value in resource referenced by Prefab instance
            bool isResourceChild = false;
            IObject * parent = _object;
            while (nullptr != parent)
            {
                if (!parent->CanOverrideProperties())
                {
                    isResourceChild = true;
                    break;
                }
                parent = parent->GetParent();
            }
            if (isResourceChild)
                return;

            m_prefab = m_gameobject->GetParentPrefab();
            m_isPrefabInstance = nullptr != m_prefab;

            if (nullptr != m_prefab)
            {
                m_canPrefabOverride = m_prefab->CanOverrideProperty(_object, _prop);

                if (m_canPrefabOverride)
                {
                    if (auto * propList = m_prefab->GetDynamicPropertyList(_object))
                    {
                        if (m_propOverride = (IDynamicProperty *)propList->GetProperty(_prop))
                        {
                            if (m_propOverride->IsEnable())
                            {
                                _object = (IObject *)m_propOverride;
                                _prop = m_propOverride->GetProperty();
                                m_isPrefabOverride = true;
                            }
                        }
                    }
                }
            }
        }
    }
}