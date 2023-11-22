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

        bool ShowSkeleton() const final override;

        void Draw(const RenderContext & _renderContext, gfx::CommandList * _cmdList) const final override;

    private:
        
    };
}