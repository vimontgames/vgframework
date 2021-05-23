namespace vg::graphics::renderer
{
    static const u32 TextureImporterDataVersion = 0;

    //--------------------------------------------------------------------------------------
    bool TextureImporterData::load(const core::string & _file)
    {
        io::Buffer buffer;

        if (io::readFile(_file, buffer))
        {
            u32 version;
            buffer.read(&version);

            if (version == MeshImporterDataVersion)
            {
                buffer.read(&name);
                buffer.read(&desc, sizeof(driver::TextureDesc));
                buffer.read(&size);

                data = (u8*)malloc(size);
                buffer.read(data, size);

                return true;
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool TextureImporterData::save(const core::string & _file) const
    {
        io::Buffer buffer;

        buffer.write(TextureImporterDataVersion);
        buffer.write(name);
        buffer.write(&desc, sizeof(driver::TextureDesc));

        buffer.write((u32)size);
        buffer.write(data, size);

        if (io::writeFile(_file, buffer))
        {
            return true;
        }

        return false;
    }

    //core::string            name;
    //driver::TextureDesc     desc;
    //core::u8 *              data = nullptr;
    //size_t                  size = 0;
}