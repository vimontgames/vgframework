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

            if (version == TextureImporterDataVersion)
            {
                buffer.read(&name);
                buffer.read(&desc, sizeof(driver::TextureDesc));

                u32 mipCount;
                buffer.read(&mipCount);
                mips.resize(mipCount);
                for (uint i = 0; i < mipCount; ++i)
                {
                    auto & mip = mips[i];

                    u32 mipSize;
                    buffer.read(&mipSize);
                    mip.buffer.resize(mipSize);
                    buffer.read(mip.buffer.data(), mipSize);
                }

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

        const u32 mipCount = (u32)mips.size();
        buffer.write(mipCount);
        for (uint i = 0; i < mipCount; ++i)
        {
            const auto & mip = mips[i];
            const u32 mipSize = (u32)mip.buffer.size();
            buffer.write(mipSize);
            buffer.write(mip.buffer.data(), mipSize);
        }

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