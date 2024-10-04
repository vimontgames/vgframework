using namespace vg::core;

namespace vg::renderer
{
    // Cooked mesh version
    static const u32 MeshImporterDataVersion = 14;

    // Use unordered_map for the flags
    static const std::unordered_map<std::string, NodeFlags> g_mixamoBoneFlags =
    {
        {"mixamorig:Hips",               NodeFlags::LowerBody},
        {"mixamorig:Spine",              NodeFlags::UpperBody},
        {"mixamorig:Spine1",             NodeFlags::UpperBody},
        {"mixamorig:Spine2",             NodeFlags::UpperBody},
        {"mixamorig:Neck",               NodeFlags::UpperBody},
        {"mixamorig:Head",               NodeFlags::UpperBody},

        // Left Arm and Hand
        {"mixamorig:LeftShoulder",       NodeFlags::UpperBody},
        {"mixamorig:LeftArm",            NodeFlags::UpperBody},
        {"mixamorig:LeftForeArm",        NodeFlags::UpperBody},
        {"mixamorig:LeftHand",           NodeFlags::UpperBody},
        {"mixamorig:LeftHandThumb1",     NodeFlags::UpperBody},
        {"mixamorig:LeftHandThumb2",     NodeFlags::UpperBody},
        {"mixamorig:LeftHandThumb3",     NodeFlags::UpperBody},
        {"mixamorig:LeftHandIndex1",     NodeFlags::UpperBody},
        {"mixamorig:LeftHandIndex2",     NodeFlags::UpperBody},
        {"mixamorig:LeftHandIndex3",     NodeFlags::UpperBody},
        {"mixamorig:LeftHandIndex_End",  NodeFlags::UpperBody},

        // Left Hand Fingers
        {"mixamorig:LeftHandMiddle1",    NodeFlags::UpperBody},
        {"mixamorig:LeftHandMiddle2",    NodeFlags::UpperBody},
        {"mixamorig:LeftHandMiddle3",    NodeFlags::UpperBody},
        {"mixamorig:LeftHandMiddle_End", NodeFlags::UpperBody},
        {"mixamorig:LeftHandRing1",      NodeFlags::UpperBody},
        {"mixamorig:LeftHandRing2",      NodeFlags::UpperBody},
        {"mixamorig:LeftHandRing3",      NodeFlags::UpperBody},
        {"mixamorig:LeftHandRing_End",   NodeFlags::UpperBody},
        {"mixamorig:LeftHandPinky1",     NodeFlags::UpperBody},
        {"mixamorig:LeftHandPinky2",     NodeFlags::UpperBody},
        {"mixamorig:LeftHandPinky3",     NodeFlags::UpperBody},
        {"mixamorig:LeftHandPinky_End",  NodeFlags::UpperBody},

        // Right Arm and Hand
        {"mixamorig:RightShoulder",      NodeFlags::UpperBody},
        {"mixamorig:RightArm",           NodeFlags::UpperBody},
        {"mixamorig:RightForeArm",       NodeFlags::UpperBody},
        {"mixamorig:RightHand",          NodeFlags::UpperBody},
        {"mixamorig:RightHandThumb1",    NodeFlags::UpperBody},
        {"mixamorig:RightHandThumb2",    NodeFlags::UpperBody},
        {"mixamorig:RightHandThumb3",    NodeFlags::UpperBody},
        {"mixamorig:RightHandIndex1",    NodeFlags::UpperBody},
        {"mixamorig:RightHandIndex2",    NodeFlags::UpperBody},
        {"mixamorig:RightHandIndex3",    NodeFlags::UpperBody},
        {"mixamorig:RightHandIndex_End", NodeFlags::UpperBody},

        // Right Hand Fingers
        {"mixamorig:RightHandMiddle1",   NodeFlags::UpperBody},
        {"mixamorig:RightHandMiddle2",   NodeFlags::UpperBody},
        {"mixamorig:RightHandMiddle3",   NodeFlags::UpperBody},
        {"mixamorig:RightHandMiddle_End",NodeFlags::UpperBody},
        {"mixamorig:RightHandRing1",     NodeFlags::UpperBody},
        {"mixamorig:RightHandRing2",     NodeFlags::UpperBody},
        {"mixamorig:RightHandRing3",     NodeFlags::UpperBody},
        {"mixamorig:RightHandRing_End",  NodeFlags::UpperBody},
        {"mixamorig:RightHandPinky1",    NodeFlags::UpperBody},
        {"mixamorig:RightHandPinky2",    NodeFlags::UpperBody},
        {"mixamorig:RightHandPinky3",    NodeFlags::UpperBody},
        {"mixamorig:RightHandPinky_End", NodeFlags::UpperBody},

        // Left Leg Bones
        {"mixamorig:LeftUpLeg",          NodeFlags::LowerBody},
        {"mixamorig:LeftLeg",            NodeFlags::LowerBody},
        {"mixamorig:LeftFoot",           NodeFlags::LowerBody},
        {"mixamorig:LeftToeBase",        NodeFlags::LowerBody},
        {"mixamorig:LeftToe_End",        NodeFlags::LowerBody},

        // Right Leg Bones
        {"mixamorig:RightUpLeg",         NodeFlags::LowerBody},
        {"mixamorig:RightLeg",           NodeFlags::LowerBody},
        {"mixamorig:RightFoot",          NodeFlags::LowerBody},
        {"mixamorig:RightToeBase",       NodeFlags::LowerBody},
        {"mixamorig:RightToe_End",       NodeFlags::LowerBody},

        // Additional Bones
        {"mixamorig:LeftEye",            NodeFlags::UpperBody},
        {"mixamorig:RightEye",           NodeFlags::UpperBody},
        {"mixamorig:Jaw",                NodeFlags::UpperBody},
        {"mixamorig:Spine3",             NodeFlags::UpperBody},
        {"mixamorig:Spine4",             NodeFlags::UpperBody}
    };

    //--------------------------------------------------------------------------------------
    void MeshImporterNode::computeNodeFlags()
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