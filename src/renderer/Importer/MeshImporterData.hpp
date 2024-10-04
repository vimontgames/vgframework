using namespace vg::core;

namespace vg::renderer
{
    // Cooked mesh version
    static const u32 MeshImporterDataVersion = 14;

    // Use unordered_map for the flags
    static const std::unordered_map<std::string, BodyPartFlags> g_mixamoBoneFlags =
    {
        {"mixamorig:Hips",               BodyPartFlags::LowerBody},
        {"mixamorig:Spine",              BodyPartFlags::UpperBody},
        {"mixamorig:Spine1",             BodyPartFlags::UpperBody},
        {"mixamorig:Spine2",             BodyPartFlags::UpperBody},
        {"mixamorig:Neck",               BodyPartFlags::UpperBody},
        {"mixamorig:Head",               BodyPartFlags::UpperBody},

        // Left Arm and Hand
        {"mixamorig:LeftShoulder",       BodyPartFlags::UpperBody},
        {"mixamorig:LeftArm",            BodyPartFlags::UpperBody},
        {"mixamorig:LeftForeArm",        BodyPartFlags::UpperBody},
        {"mixamorig:LeftHand",           BodyPartFlags::UpperBody},
        {"mixamorig:LeftHandThumb1",     BodyPartFlags::UpperBody},
        {"mixamorig:LeftHandThumb2",     BodyPartFlags::UpperBody},
        {"mixamorig:LeftHandThumb3",     BodyPartFlags::UpperBody},
        {"mixamorig:LeftHandIndex1",     BodyPartFlags::UpperBody},
        {"mixamorig:LeftHandIndex2",     BodyPartFlags::UpperBody},
        {"mixamorig:LeftHandIndex3",     BodyPartFlags::UpperBody},
        {"mixamorig:LeftHandIndex_End",  BodyPartFlags::UpperBody},

        // Left Hand Fingers
        {"mixamorig:LeftHandMiddle1",    BodyPartFlags::UpperBody},
        {"mixamorig:LeftHandMiddle2",    BodyPartFlags::UpperBody},
        {"mixamorig:LeftHandMiddle3",    BodyPartFlags::UpperBody},
        {"mixamorig:LeftHandMiddle_End", BodyPartFlags::UpperBody},
        {"mixamorig:LeftHandRing1",      BodyPartFlags::UpperBody},
        {"mixamorig:LeftHandRing2",      BodyPartFlags::UpperBody},
        {"mixamorig:LeftHandRing3",      BodyPartFlags::UpperBody},
        {"mixamorig:LeftHandRing_End",   BodyPartFlags::UpperBody},
        {"mixamorig:LeftHandPinky1",     BodyPartFlags::UpperBody},
        {"mixamorig:LeftHandPinky2",     BodyPartFlags::UpperBody},
        {"mixamorig:LeftHandPinky3",     BodyPartFlags::UpperBody},
        {"mixamorig:LeftHandPinky_End",  BodyPartFlags::UpperBody},

        // Right Arm and Hand
        {"mixamorig:RightShoulder",      BodyPartFlags::UpperBody},
        {"mixamorig:RightArm",           BodyPartFlags::UpperBody},
        {"mixamorig:RightForeArm",       BodyPartFlags::UpperBody},
        {"mixamorig:RightHand",          BodyPartFlags::UpperBody},
        {"mixamorig:RightHandThumb1",    BodyPartFlags::UpperBody},
        {"mixamorig:RightHandThumb2",    BodyPartFlags::UpperBody},
        {"mixamorig:RightHandThumb3",    BodyPartFlags::UpperBody},
        {"mixamorig:RightHandIndex1",    BodyPartFlags::UpperBody},
        {"mixamorig:RightHandIndex2",    BodyPartFlags::UpperBody},
        {"mixamorig:RightHandIndex3",    BodyPartFlags::UpperBody},
        {"mixamorig:RightHandIndex_End", BodyPartFlags::UpperBody},

        // Right Hand Fingers
        {"mixamorig:RightHandMiddle1",   BodyPartFlags::UpperBody},
        {"mixamorig:RightHandMiddle2",   BodyPartFlags::UpperBody},
        {"mixamorig:RightHandMiddle3",   BodyPartFlags::UpperBody},
        {"mixamorig:RightHandMiddle_End",BodyPartFlags::UpperBody},
        {"mixamorig:RightHandRing1",     BodyPartFlags::UpperBody},
        {"mixamorig:RightHandRing2",     BodyPartFlags::UpperBody},
        {"mixamorig:RightHandRing3",     BodyPartFlags::UpperBody},
        {"mixamorig:RightHandRing_End",  BodyPartFlags::UpperBody},
        {"mixamorig:RightHandPinky1",    BodyPartFlags::UpperBody},
        {"mixamorig:RightHandPinky2",    BodyPartFlags::UpperBody},
        {"mixamorig:RightHandPinky3",    BodyPartFlags::UpperBody},
        {"mixamorig:RightHandPinky_End", BodyPartFlags::UpperBody},

        // Left Leg Bones
        {"mixamorig:LeftUpLeg",          BodyPartFlags::LowerBody},
        {"mixamorig:LeftLeg",            BodyPartFlags::LowerBody},
        {"mixamorig:LeftFoot",           BodyPartFlags::LowerBody},
        {"mixamorig:LeftToeBase",        BodyPartFlags::LowerBody},
        {"mixamorig:LeftToe_End",        BodyPartFlags::LowerBody},

        // Right Leg Bones
        {"mixamorig:RightUpLeg",         BodyPartFlags::LowerBody},
        {"mixamorig:RightLeg",           BodyPartFlags::LowerBody},
        {"mixamorig:RightFoot",          BodyPartFlags::LowerBody},
        {"mixamorig:RightToeBase",       BodyPartFlags::LowerBody},
        {"mixamorig:RightToe_End",       BodyPartFlags::LowerBody},

        // Additional Bones
        {"mixamorig:LeftEye",            BodyPartFlags::UpperBody},
        {"mixamorig:RightEye",           BodyPartFlags::UpperBody},
        {"mixamorig:Jaw",                BodyPartFlags::UpperBody},
        {"mixamorig:Spine3",             BodyPartFlags::UpperBody},
        {"mixamorig:Spine4",             BodyPartFlags::UpperBody}
    };

    //--------------------------------------------------------------------------------------
    void MeshImporterNode::computeBodyPartFlags()
    {
        auto it = g_mixamoBoneFlags.find(name);
            
        if (g_mixamoBoneFlags.end() != it)
            flags |= it->second;
    }

    //--------------------------------------------------------------------------------------
    bool MeshImporterNode::read(core::io::Buffer & _buffer)
    {
        _buffer.read(&name);
        _buffer.read(&parent_index);
        _buffer.read(&flags);

        _buffer.read(&geometry_to_node);
        _buffer.read(&node_to_parent);
        _buffer.read(&node_to_world);
        _buffer.read(&geometry_to_world);
        _buffer.read(&normal_to_world);

        _buffer.read(&rot);
        _buffer.read(&pos);
        _buffer.read(&scale);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MeshImporterNode::write(core::io::Buffer & _buffer) const
    {
        _buffer.write(name);
        _buffer.write(parent_index);
        _buffer.write(flags);

        _buffer.write(geometry_to_node);
        _buffer.write(node_to_parent);
        _buffer.write(node_to_world);
        _buffer.write(geometry_to_world);
        _buffer.write(normal_to_world);

        _buffer.write(rot);
        _buffer.write(pos);
        _buffer.write(scale);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MeshImporterData::load(const core::string & _file)
    {
        io::Buffer buffer;

        if (io::readFile(_file, buffer))
        {
            u32 version;
            buffer.read(&version);

            if (version == MeshImporterDataVersion)
            {
                buffer.read(&name);

                aabb.read(buffer);

                u32 batchCount;
                buffer.read(&batchCount);
                batches.resize(batchCount);
                for (uint i = 0; i < batchCount; ++i)
                    batches[i].read(buffer);

                buffer.read(&indices);
                buffer.read(&vertices);

                u32 matCount;
                buffer.read(&matCount);
                materials.resize(matCount);
                for (uint i = 0; i < matCount; ++i)
                    materials[i].read(buffer);

                buffer.read(&maxBonesCountPerVertex);
                
                u32 nodeCount;
                buffer.read(&nodeCount);
                nodes.resize(nodeCount);
                for (uint i = 0; i < nodeCount; ++i)
                    nodes[i].read(buffer);

                buffer.read(&bonesIndices);
                buffer.read(&bonesMatrices);

                return true;
            }
        }        

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool MeshImporterData::save(const core::string & _file) const
    {
        io::Buffer buffer;

        buffer.write(MeshImporterDataVersion);

        buffer.write(name);
        aabb.write(buffer);

        buffer.write((u32)batches.size());
        for (uint i = 0; i < batches.size(); ++i)
            batches[i].write(buffer);

        buffer.write(indices);
        buffer.write(vertices);

        buffer.write((u32)materials.size());
        for (uint i = 0; i < materials.size(); ++i)
            materials[i].write(buffer);

        buffer.write(maxBonesCountPerVertex);
        
        buffer.write((u32)nodes.size());
        for (uint i = 0; i < nodes.size(); ++i)
            nodes[i].write(buffer);
        
        buffer.write(bonesIndices);
        buffer.write(bonesMatrices);

        if (io::writeFile(_file, buffer))
            return true;

        return false;
    }
}