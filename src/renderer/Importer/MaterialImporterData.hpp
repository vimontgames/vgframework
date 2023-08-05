namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    bool MaterialImporterData::read(core::io::Buffer & _buffer)
    {
        _buffer.read(&name);

        u32 texCount;
        if (_buffer.read(&texCount))
        {
            VG_ASSERT(texCount <= countof(texturePath));
            for (uint i = 0; i < texCount; ++i)
                _buffer.read(&texturePath[i]);

            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool MaterialImporterData::write(core::io::Buffer & _buffer) const
    {
        _buffer.write(name);

        const u32 texCount = (u32)countof(texturePath);
        _buffer.write(texCount);
        for (uint i = 0; i < texCount; ++i)
            _buffer.write(texturePath[i]);

        return true;
    }
}