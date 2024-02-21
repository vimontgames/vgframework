#pragma once

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    // Parent should have the union of its children update flags (e.g. a GameObject should 'OR' the flags of all its children)
    //--------------------------------------------------------------------------------------
    enum class UpdateFlags : core::u8
    {
        FixedUpdate     = 0x01, // 'FixedUpdate' should be used for physics-based calculations such as applying forces to Rigidbodies or any other calculations needing to be framerate independent
        Update          = 0x02, // 'Update' will be called once per frame
        LateUpdate      = 0x04, // 'LateUpdate' will also be called once per frame, after all other 'Updates' and should be used to e.g. animate model bones for lookup 
        InstancedPrefab = 0x08  // GameObject is a Prefab that has been instanced
    };
}