#pragma once

#include "graphics/renderer/IMeshInstance.h"

namespace vg::core
{
    class IFactory;
    class IClassDesc;
}

namespace vg::graphics::renderer
{
    class MeshInstance : public IMeshInstance
    {
    public:
        using super = IMeshInstance;

        const char * getClassName() const final { return "MeshInstance"; }

        MeshInstance(const core::string & _name, core::IObject * _parent);
        ~MeshInstance();

        static bool registerClass(core::IFactory & _factory);
        static bool registerProperties(core::IClassDesc & _desc);

    private:
        
    };
}