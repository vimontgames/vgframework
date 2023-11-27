using namespace vg::core;

namespace vg::renderer
{
    // Cooked anim version
    static const u32 AnimImporterDataVersion = 2;

    //--------------------------------------------------------------------------------------
    bool AnimNodeData::read(core::io::Buffer & _buffer)
    {
        _buffer.read(&name);
        _buffer.read(&time_begin);
        _buffer.read(&framerate);
        _buffer.read(&num_frames);
        _buffer.read(&const_rot);
        _buffer.read(&const_pos);
        _buffer.read(&const_scale);
        _buffer.read(&rot);
        _buffer.read(&pos);
        _buffer.read(&scale);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool AnimNodeData::write(core::io::Buffer & _buffer) const
    {
        _buffer.write(name);
        _buffer.write(time_begin);
        _buffer.write(framerate);
        _buffer.write(num_frames);
        _buffer.write(const_rot);
        _buffer.write(const_pos);
        _buffer.write(const_scale);
        _buffer.write(rot);
        _buffer.write(pos);
        _buffer.write(scale);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool AnimImporterData::load(const core::string & _file)
    {
        io::Buffer buffer;

        if (io::readFile(_file, buffer))
        {
            u32 version;
            buffer.read(&version);

            if (version == AnimImporterDataVersion)
            {
                buffer.read(&name);
                buffer.read(&time_begin);
                buffer.read(&time_end);
                buffer.read(&framerate);
                buffer.read(&num_frames);

                u32 animNodesCount;
                buffer.read(&animNodesCount);
                animNodes.resize(animNodesCount);
                for (uint i = 0; i < animNodesCount; ++i)
                    animNodes[i].read(buffer);

                return true;
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool AnimImporterData::save(const core::string & _file) const
    {
        io::Buffer buffer;

        buffer.write(AnimImporterDataVersion);

        buffer.write(name);
        buffer.write(time_begin);
        buffer.write(time_end);
        buffer.write(framerate);
        buffer.write(num_frames);

        buffer.write((u32)animNodes.size());
        for (uint i = 0; i < animNodes.size(); ++i)
            animNodes[i].write(buffer);

        if (io::writeFile(_file, buffer))
            return true;

        return false;
    }
}