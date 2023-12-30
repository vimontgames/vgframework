#pragma once

namespace vg::renderer
{
    class IGraphicInstance;

    enum class GraphicInstanceListType : core::u8
    {
        All = 0,
        Opaque,
        Transparent,
        Light
    };

    class GraphicInstanceList
    {
    public:
        core::vector<const IGraphicInstance *> m_instances;

        void clear()
        {
            m_instances.clear();
        }
    };
}