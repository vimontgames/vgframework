using namespace vg::core;

namespace vg::renderer
{
    static const u32 MeshImporterDataVersion = 2;

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

        if (io::writeFile(_file, buffer))
        {
            return true;
        }

        return false;
    }
}