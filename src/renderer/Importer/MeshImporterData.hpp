#include "core/string/string.h"
#include "NamingConvention/MixamoBonesNamingConvention.h"
#include "NamingConvention/CharacterStudioBipedNamingConvention.h"

using namespace vg::core;

namespace vg::renderer
{
    // Cooked mesh version
    static const u32 MeshImporterDataVersion = 19;

    //--------------------------------------------------------------------------------------
    bool findBodyPartFlagsFromNamingConvention(const core::string & _name, const vector<BonesNamingConvention> & _namingConvention, BodyPartFlags & _flags)
    {
        string name = _name;

        for (uint i = 0; i < _namingConvention.size(); ++i)
        {
            if (endsWith(name, _namingConvention[i].name))
            {
                _flags = _namingConvention[i].flags;
                return true;
            }
        }

        // Retry removing trailing "_end"
        const std::string suffix = "_end";

        // Check if string ends with "_end"
        if (name.size() >= suffix.size() &&
            name.compare(name.size() - suffix.size(), suffix.size(), suffix) == 0) {
            name.erase(name.size() - suffix.size()); // remove the suffix
        }

        for (uint i = 0; i < _namingConvention.size(); ++i)
        {
            if (endsWith(name, _namingConvention[i].name))
            {
                _flags = _namingConvention[i].flags;
                return true;
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool MeshImporterNode::computeBodyPartFlags()
    {
        BodyPartFlags namingFlags;

        if (findBodyPartFlagsFromNamingConvention(name, g_mixamoBoneFlags, namingFlags))
        {
            flags = namingFlags;
            return true;
        }

        if (findBodyPartFlagsFromNamingConvention(name, g_characterStudioBoneFlags, namingFlags))
        {
            flags = namingFlags;
            return true;
        }

        // If bone is not matching any naming convention, assume it belongs to all categories
        flags = (BodyPartFlags)-1;
        VG_WARNING("[FBX] Could not find naming convention for node \"%s\"", name.c_str());
        return false;
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
    LoadStatus MeshImporterData::load(const core::string & _file)
    {
        io::Buffer buffer;

        if (io::readFile(_file, buffer))
        {
            u32 version;
            buffer.read(&version);

            if (version != MeshImporterDataVersion)
                return LoadStatus::Deprecated;
            
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

            buffer.read(&colliderTriangles);

            return LoadStatus::Success;
        }        

        return LoadStatus::CannotOpenFile;
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

        buffer.write(colliderTriangles);

        if (io::writeFile(_file, buffer))
            return true;

        return false;
    }
}