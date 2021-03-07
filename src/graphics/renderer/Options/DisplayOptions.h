#pragma once

#include "core/Object/Object.h"
#include "core/Singleton/Singleton.h"

namespace vg::core
{
    class IObjectFactory;
}

namespace vg::graphics::renderer
{
    class DisplayOptions : public core::Object, public core::Singleton<DisplayOptions>
    {
    public:
        const char *    getClassName    () const final { return "DisplayOptions"; }
        static bool     registerClass   (core::IObjectFactory & _factory);

        core::float4    getBackgroundColor() { return m_backgroundColor; }

    private:
        core::float4    m_backgroundColor = core::float4(0, 0, 0, 0);
        bool            m_displayNormals = false;
    };
}