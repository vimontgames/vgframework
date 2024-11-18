#pragma once

namespace vg::core
{
    class IObject;
    class IGameObject;
    class IProperty;
    class IDynamicProperty;
}

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    // Edition context for IObject
    //--------------------------------------------------------------------------------------
    struct ObjectContext
    {
        struct TreeNodeStackInfo
        {
            bool treeNodeIsCollapsingHeader = false;
            bool treeNodeOpen = false;
            bool treeNodeContentDisabled = false;
        };

        float                           m_availableWidth = 0.0f;
        core::vector<TreeNodeStackInfo> m_treeNodes;
        bool                            m_hide = false;
    };

    //--------------------------------------------------------------------------------------
    // Edition context for IProperty
    //--------------------------------------------------------------------------------------
    struct PropertyContext
    {
        PropertyContext(core::IObject *& _object, const core::IProperty *& _prop);

        bool                        m_readOnly              = false;
        bool                        m_isPrefabInstance      = false;    // Property is from an instanced Prefab (cannot be edited directly)
        bool                        m_isPrefabOverride      = false;    // Prefab has overrides
        bool                        m_canPrefabOverride     = false;    // Property can be override for this Prefab Instance
                                                                        
        core::IGameObject *         m_gameobject            = nullptr;  
        core::IObject *             m_originalObject        = nullptr;
        const core::IProperty *     m_originalProp          = nullptr;
        core::IGameObject *         m_prefab                = nullptr;  // Original Prefab
        core::IDynamicProperty *    m_propOverride          = nullptr;  // The dynamic property override if any

        core::IObject *             m_optionalObject        = nullptr;
        const core::IProperty *     m_optionalProp          = nullptr;
        core::IDynamicProperty *    m_optionalPropOverride  = nullptr;
    };
}