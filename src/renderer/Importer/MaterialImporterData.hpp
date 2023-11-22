
using namespace vg::core;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    MaterialImporterData::MaterialImporterData()
    {
        for (uint i = 0; i < enumCount<MaterialColorType>(); ++i)
            m_colors[i] = core::float4(0, 0, 0, 0);
        m_colors[asInteger(MaterialColorType::BaseColor)] = float4(1, 1, 1, 1);
    }

    //--------------------------------------------------------------------------------------
    bool MaterialImporterData::read(io::Buffer & _buffer)
    {
        _buffer.read(&m_name);

        u32 texCount;
        if (_buffer.read(&texCount))
        {
            VG_ASSERT(texCount <= countof(m_texturePath));
            for (uint i = 0; i < texCount; ++i)
                _buffer.read(&m_texturePath[i]);
        }

        u32 colorCount;
        if (_buffer.read(&colorCount))
        {
            VG_ASSERT(colorCount <= countof(m_colors));
            for (uint i = 0; i < colorCount; ++i)
                _buffer.read(&m_colors[i]);
        }

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MaterialImporterData::write(io::Buffer & _buffer) const
    {
        _buffer.write(m_name);

        const u32 texCount = (u32)countof(m_texturePath);
        _buffer.write(texCount);
        for (uint i = 0; i < texCount; ++i)
            _buffer.write(m_texturePath[i]);

        const u32 colorCount = (u32)countof(m_colors);
        _buffer.write(colorCount);
        for (uint i = 0; i < colorCount; ++i)
            _buffer.write(m_colors[i]);

        return true;
    }
}