#pragma once

namespace vg::renderer
{
    template <typename T> class InstanceList
    {
    public:
        core::vector<T*> m_instances;

        inline void clear()
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

    class CameraInstanceList : public InstanceList<class ICameraInstance>
    {

    };
}