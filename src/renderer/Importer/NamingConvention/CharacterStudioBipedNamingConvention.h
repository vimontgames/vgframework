namespace vg::renderer
{
    static const vector<BonesNamingConvention> g_characterStudioBoneFlags =
    {
        // Right Leg
        {"R Thigh",               BodyPartFlags::LowerBody},
        {"R Calf",                BodyPartFlags::LowerBody},
        {"R Foot",                BodyPartFlags::LowerBody},
        {"R Toe0",                BodyPartFlags::LowerBody},
        {"R Toe0Nub",             BodyPartFlags::LowerBody},

        // Left Leg
        {"L Thigh",               BodyPartFlags::LowerBody},
        {"L Calf",                BodyPartFlags::LowerBody},
        {"L Foot",                BodyPartFlags::LowerBody},
        {"L Toe0",                BodyPartFlags::LowerBody},
        {"L Toe0Nub",             BodyPartFlags::LowerBody},

        // Right Arm
        {"R Clavicle",            BodyPartFlags::UpperBody},
        {"R UpperArm",            BodyPartFlags::UpperBody},
        {"R Forearm",             BodyPartFlags::UpperBody},
        {"R Hand",                BodyPartFlags::UpperBody},

        // Left Arm
        {"L Clavicle",            BodyPartFlags::UpperBody},
        {"L UpperArm",            BodyPartFlags::UpperBody},
        {"L Forearm",             BodyPartFlags::UpperBody},
        {"L Hand",                BodyPartFlags::UpperBody},

        // Right Hand Fingers
        {"R Finger0",             BodyPartFlags::UpperBody},
        {"R Finger1",             BodyPartFlags::UpperBody},
        {"R Finger2",             BodyPartFlags::UpperBody},
        {"R Finger3",             BodyPartFlags::UpperBody},
        {"R Finger4",             BodyPartFlags::UpperBody},

        // Left Hand Fingers
        {"L Finger0",             BodyPartFlags::UpperBody},
        {"L Finger1",             BodyPartFlags::UpperBody},
        {"L Finger2",             BodyPartFlags::UpperBody},
        {"L Finger3",             BodyPartFlags::UpperBody},
        {"L Finger4",             BodyPartFlags::UpperBody},

        // Right Hand Nub Fingers
        {"R Finger0Nub",          BodyPartFlags::UpperBody},
        {"R Finger1Nub",          BodyPartFlags::UpperBody},
        {"R Finger2Nub",          BodyPartFlags::UpperBody},
        {"R Finger3Nub",          BodyPartFlags::UpperBody},
        {"R Finger4Nub",          BodyPartFlags::UpperBody},

        // Left Hand Nub Fingers
        {"L Finger0Nub",          BodyPartFlags::UpperBody},
        {"L Finger1Nub",          BodyPartFlags::UpperBody},
        {"L Finger2Nub",          BodyPartFlags::UpperBody},
        {"L Finger3Nub",          BodyPartFlags::UpperBody},
        {"L Finger4Nub",          BodyPartFlags::UpperBody},

        // Additional right fingers
        {"R Finger01",            BodyPartFlags::UpperBody},
        {"R Finger11",            BodyPartFlags::UpperBody},
        {"R Finger21",            BodyPartFlags::UpperBody},
        {"R Finger31",            BodyPartFlags::UpperBody},
        {"R Finger41",            BodyPartFlags::UpperBody},

        // Additional left fingers
        {"L Finger01",            BodyPartFlags::UpperBody},
        {"L Finger11",            BodyPartFlags::UpperBody},
        {"L Finger21",            BodyPartFlags::UpperBody},
        {"L Finger31",            BodyPartFlags::UpperBody},
        {"L Finger41",            BodyPartFlags::UpperBody},

        // Additional fingers with numbered variations
        {"R Finger02",            BodyPartFlags::UpperBody},
        {"R Finger12",            BodyPartFlags::UpperBody},
        {"R Finger22",            BodyPartFlags::UpperBody},
        {"R Finger32",            BodyPartFlags::UpperBody},
        {"R Finger42",            BodyPartFlags::UpperBody},

        {"L Finger02",            BodyPartFlags::UpperBody},
        {"L Finger12",            BodyPartFlags::UpperBody},
        {"L Finger22",            BodyPartFlags::UpperBody},
        {"L Finger32",            BodyPartFlags::UpperBody},
        {"L Finger42",            BodyPartFlags::UpperBody},

        // Footsteps
        {"Footsteps",             BodyPartFlags::LowerBody},

        // Pelvis
        {"Pelvis",                BodyPartFlags::LowerBody | BodyPartFlags::UpperBody},

        // Head Nub
        {"HeadNub",               BodyPartFlags::UpperBody},
    };
}