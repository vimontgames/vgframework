namespace vg::renderer
{
    static const vector<BonesNamingConvention> g_mixamoBoneFlags =
    {
        {"Hips",               BodyPartFlags::LowerBody},
        {"Spine",              BodyPartFlags::UpperBody},
        {"Spine1",             BodyPartFlags::UpperBody},
        {"Spine2",             BodyPartFlags::UpperBody},
        {"Neck",               BodyPartFlags::UpperBody},
        {"Head",               BodyPartFlags::UpperBody},
        {"HeadTop_End",        BodyPartFlags::UpperBody}, // Added HeadTop_End

        // Left Arm and Hand
        {"LeftShoulder",       BodyPartFlags::UpperBody},
        {"LeftArm",            BodyPartFlags::UpperBody},
        {"LeftForeArm",        BodyPartFlags::UpperBody},
        {"LeftHand",           BodyPartFlags::UpperBody},
        {"LeftHandThumb1",     BodyPartFlags::UpperBody},
        {"LeftHandThumb2",     BodyPartFlags::UpperBody},
        {"LeftHandThumb3",     BodyPartFlags::UpperBody},
        {"LeftHandThumb4",     BodyPartFlags::UpperBody}, // Added LeftHandThumb4
        {"LeftHandIndex1",     BodyPartFlags::UpperBody},
        {"LeftHandIndex2",     BodyPartFlags::UpperBody},
        {"LeftHandIndex3",     BodyPartFlags::UpperBody},
        {"LeftHandIndex4",     BodyPartFlags::UpperBody}, // Added LeftHandIndex4
        {"LeftHandIndex_End",  BodyPartFlags::UpperBody},

        // Left Hand Fingers
        {"LeftHandMiddle1",    BodyPartFlags::UpperBody},
        {"LeftHandMiddle2",    BodyPartFlags::UpperBody},
        {"LeftHandMiddle3",    BodyPartFlags::UpperBody},
        {"LeftHandMiddle4",    BodyPartFlags::UpperBody}, // Added LeftHandMiddle4
        {"LeftHandMiddle_End", BodyPartFlags::UpperBody},
        {"LeftHandRing1",      BodyPartFlags::UpperBody},
        {"LeftHandRing2",      BodyPartFlags::UpperBody},
        {"LeftHandRing3",      BodyPartFlags::UpperBody},
        {"LeftHandRing4",      BodyPartFlags::UpperBody}, // Added LeftHandRing4
        {"LeftHandRing_End",   BodyPartFlags::UpperBody},
        {"LeftHandPinky1",     BodyPartFlags::UpperBody},
        {"LeftHandPinky2",     BodyPartFlags::UpperBody},
        {"LeftHandPinky3",     BodyPartFlags::UpperBody},
        {"LeftHandPinky4",     BodyPartFlags::UpperBody}, // Added LeftHandPinky4
        {"LeftHandPinky_End",  BodyPartFlags::UpperBody},

        // Right Arm and Hand
        {"RightShoulder",      BodyPartFlags::UpperBody},
        {"RightArm",           BodyPartFlags::UpperBody},
        {"RightForeArm",       BodyPartFlags::UpperBody},
        {"RightHand",          BodyPartFlags::UpperBody},
        {"RightHandThumb1",    BodyPartFlags::UpperBody},
        {"RightHandThumb2",    BodyPartFlags::UpperBody},
        {"RightHandThumb3",    BodyPartFlags::UpperBody},
        {"RightHandThumb4",    BodyPartFlags::UpperBody}, // Added RightHandThumb4
        {"RightHandIndex1",    BodyPartFlags::UpperBody},
        {"RightHandIndex2",    BodyPartFlags::UpperBody},
        {"RightHandIndex3",    BodyPartFlags::UpperBody},
        {"RightHandIndex4",    BodyPartFlags::UpperBody}, // Added RightHandIndex4
        {"RightHandIndex_End", BodyPartFlags::UpperBody},

        // Right Hand Fingers
        {"RightHandMiddle1",   BodyPartFlags::UpperBody},
        {"RightHandMiddle2",   BodyPartFlags::UpperBody},
        {"RightHandMiddle3",   BodyPartFlags::UpperBody},
        {"RightHandMiddle4",   BodyPartFlags::UpperBody}, // Added RightHandMiddle4
        {"RightHandMiddle_End",BodyPartFlags::UpperBody},
        {"RightHandRing1",     BodyPartFlags::UpperBody},
        {"RightHandRing2",     BodyPartFlags::UpperBody},
        {"RightHandRing3",     BodyPartFlags::UpperBody},
        {"RightHandRing4",     BodyPartFlags::UpperBody}, // Added RightHandRing4
        {"RightHandRing_End",  BodyPartFlags::UpperBody},
        {"RightHandPinky1",    BodyPartFlags::UpperBody},
        {"RightHandPinky2",    BodyPartFlags::UpperBody},
        {"RightHandPinky3",    BodyPartFlags::UpperBody},
        {"RightHandPinky4",    BodyPartFlags::UpperBody}, // Added RightHandPinky4
        {"RightHandPinky_End", BodyPartFlags::UpperBody},

        // Left Leg Bones
        {"LeftUpLeg",          BodyPartFlags::LowerBody},
        {"LeftLeg",            BodyPartFlags::LowerBody},
        {"LeftFoot",           BodyPartFlags::LowerBody},
        {"LeftToeBase",        BodyPartFlags::LowerBody},
        {"LeftToe_End",        BodyPartFlags::LowerBody},

        // Right Leg Bones
        {"RightUpLeg",         BodyPartFlags::LowerBody},
        {"RightLeg",           BodyPartFlags::LowerBody},
        {"RightFoot",          BodyPartFlags::LowerBody},
        {"RightToeBase",       BodyPartFlags::LowerBody},
        {"RightToe_End",       BodyPartFlags::LowerBody},

        // Additional Bones
        {"LeftEye",            BodyPartFlags::UpperBody},
        {"RightEye",           BodyPartFlags::UpperBody},
        {"Jaw",                BodyPartFlags::UpperBody},
        {"Spine3",             BodyPartFlags::UpperBody},
        {"Spine4",             BodyPartFlags::UpperBody}
    };
}