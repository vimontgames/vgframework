#pragma once

#include "renderer/IMeshInstance.h"

namespace vg::core
{
    class IFactory;
    class IClassDesc;
    class IModel;
    enum class Lod : core::u8;
}

namespace vg::renderer
{
    class DisplayOptions;
    class Skeleton;

    class MeshInstance : public IMeshInstance
    {
    public:
        VG_CLASS_DECL(MeshInstance, IMeshInstance);

        MeshInstance(const core::string & _name, core::IObject * _parent);
        ~MeshInstance();

        void                SetModel            (core::Lod _lod, core::IModel * _model) final override;
        bool                ShowSkeleton        () const final override;

        void                Draw                (const RenderContext & _renderContext, gfx::CommandList * _cmdList) const final override;

        bool                setInstanceSkeleton (const Skeleton * _skeleton);
        const Skeleton *    getInstanceSkeleton() const;

    private:
        Skeleton *          m_instanceSkeleton = nullptr;
    };
}