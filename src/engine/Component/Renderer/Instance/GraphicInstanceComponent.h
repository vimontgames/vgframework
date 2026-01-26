#pragma once
#include "core/Component/Component.h"
#include "renderer/Renderer_consts.h"

namespace vg::renderer
{
    class IGraphicInstance;
}

namespace vg::engine
{
    class AnimationJob;
    class DrawSkeletonJob;

    class GraphicInstanceComponent : public core::Component
    {
    public:
        VG_CLASS_DECL(GraphicInstanceComponent, core::Component);

        GraphicInstanceComponent(const core::string & _name, core::IObject * _parent);
        ~GraphicInstanceComponent();

        virtual void                            SetOutlineCategory          (renderer::OutlineCategory _category);
        virtual renderer::OutlineCategory       GetOutlineCategory          () const;

    protected:
        void                                    OnLoad                      () override;

        void                                    OnEnable                    () override;
        void                                    OnDisable                   () override;

        // When GameObject's flags change, propagate flags to renderer graphic instance (e.g. 'Static')
        bool                                    UpdateFlagsFromGameObject   () override;

        // For refresh when any property changes
        void                                    OnPropertyChanged           (core::IObject * _object, const core::IProperty & _prop, bool _notifyParent) override;

        // Get the renderer graphic instance the component is using for render (e.g. MeshInstance, ParticleSystemInstance ...)
        virtual renderer::IGraphicInstance *    GetGraphicInstance          () = 0;

        // Refresh the renderer's graphic instance using GameObject and Component settings
        virtual void                            RefreshGraphicInstance      () = 0;

    protected:
        bool                                    registerGraphicInstance     ();
        bool                                    unregisterGraphicInstance   ();

    protected:
        bool                                    m_registered = false;
        renderer::OutlineCategory               m_outlineCategory = (renderer::OutlineCategory)0;
    };
}