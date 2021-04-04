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
        using super = core::Object;

                        DisplayOptions      (const core::string & _name, core::IObject * _parent);

        const char *    getClassName        () const final { return "DisplayOptions"; }

        static bool     registerClass       (core::IObjectFactory & _factory);
        static bool     registerProperties  (core::IObjectDescriptor & _desc);

        bool            isToolModeEnabled   () const { return m_toolMode; }
        bool            isWireframeEnabled  () const { return m_wireframe; }
        bool            isNormalEnabled     () const { return m_displayNormals; }

        core::float4    getBackgroundColor  () const { return m_backgroundColor; }

        static bool     load                (IObject * _object);
        static bool     save                (IObject * _object);

    private:
        core::float4    m_backgroundColor   = core::float4(0, 0, 0, 0);
        bool            m_toolMode          = true;
        bool            m_wireframe         = false;
        bool            m_displayNormals    = false;
    };
}