#include "TextureImporterData.h"

namespace vg::renderer
{
    static const u32 TextureImporterDataVersion = 2;

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
                buffer.read(&desc, sizeof(gfx::TextureDesc));

                u32 size;
                buffer.read(&size);
                texels.resize(size);

                buffer.read(texels.data(), size);

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
        buffer.write(&desc, sizeof(gfx::TextureDesc));

        const u32 size = (u32)texels.size();
        buffer.write(size);
        buffer.write(texels.data(), size);

        if (io::writeFile(_file, buffer))
            return true;

        return false;
    }
}