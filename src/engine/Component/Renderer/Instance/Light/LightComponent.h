#pragma once

#include "engine/Component/Renderer/Instance/GraphicInstanceComponent.h"

namespace vg::renderer
{
    enum class LightType : core::u8;
    class ILightDesc;
    class ILightInstance;
}

namespace vg::engine
{
    class LightComponent final : public GraphicInstanceComponent
    {
    public:
        VG_CLASS_DECL(LightComponent, GraphicInstanceComponent);

        LightComponent(const core::string & _name, core::IObject * _parent);
        ~LightComponent();

        void                            OnLoad                  () final override;
        void                            OnEnable                () final override;
        void                            OnDisable               () final override;
        void                            Update                  (const Context & _context) final override;
        void                            OnPropertyChanged       (core::IObject * _object, const core::IProperty & _prop, bool _notifyParent) final override;
        void                            EnableComponentFlags    (core::ComponentFlags _flags, bool _enabled = true) final override;
        bool                            TryGetAABB              (core::AABB & _aabb) const final override;

        renderer::IGraphicInstance *    GetGraphicInstance      () final override;
        void                            RefreshGraphicInstance  () final override;

    protected:
        bool                            createLightDesc         ();
        bool                            createLight             ();

    private:
        renderer::LightType             m_lightType;
        renderer::ILightDesc *          m_lightDesc     = nullptr;
        renderer::ILightInstance *      m_light         = nullptr;
    };
}

#if VG_ENABLE_INLINE
#include "LightComponent.inl"
#endif