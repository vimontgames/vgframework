#pragma once

#include "renderer/IMeshInstance.h"

namespace vg::core
{
    class IFactory;
    class IClassDesc;
}

namespace vg::renderer
{
    class DisplayOptions;

    class MeshInstance : public IMeshInstance
    {
    public:
        using super = IMeshInstance;

        const char * getClassName() const final { return "MeshInstance"; }

        MeshInstance(const core::string & _name, core::IObject * _parent);
        ~MeshInstance();

        static bool registerClass(core::IFactory & _factory);
        static bool registerProperties(core::IClassDesc & _desc);

        void Draw(const RenderContext & _renderContext, gfx::CommandList * _cmdList) const final override;

        static core::u16 configureToolMode(const DisplayOptions & _options, core::u16 _mode);
        static core::u16 configureToolModeFlags(const DisplayOptions & _options, core::u16 _flags);

    private:
        
    };
}