#pragma once

namespace vg::renderer
{
    template <typename T> class InstanceList
    {
    public:
        core::vector<const T *> m_instances;

        void clear()
        {
            m_instances.clear();
        }
    };

    class GraphicInstanceList : public InstanceList<class GraphicInstance>
    {

    };

    class LightInstanceList : public InstanceList<class LightInstance>
    {

    };
}