#pragma once
#include "core/Singleton/Singleton.h"
#include "editor/IEditorOptions.h"
#include "renderer/IImGuiAdapter.h"

namespace vg::editor
{
    enum class GizmoType : core::u8
    {
        Translate = 0,
        Rotate,
        Scale
    };

    enum class GizmoSpace : core::u8
    {
        Local = 0,
        World
    };

    struct GizmoOptions
    {
        GizmoType           m_type = GizmoType::Translate;
        GizmoSpace          m_space = GizmoSpace::World;

        bool                m_snapTranslate = true;
        core::float3        m_translationSnap = core::float3(1, 1, 1);

        bool                m_snapRotate = true;
        float               m_rotationSnapInDegrees = 90.0f / 4.0f;

        bool                m_snapScale = true;
        float               m_scaleSnap = 1.0f;
    };

    class EditorOptions final : public IEditorOptions, public core::Singleton<EditorOptions>
    {
    public:
        VG_CLASS_DECL(EditorOptions, IEditorOptions);

        EditorOptions(const core::string & _name, core::IObject * _parent = nullptr);

        void                    OnPropertyChanged   (IObject * _object, const core::IProperty & _prop, bool _notifyParent) final override;

        bool                    setGizmoType        (GizmoType _gizmoType);
        bool                    setSnap             (bool _enable);
        bool                    getSnap             () const;

        const GizmoOptions &    getGizmoOptions     () const { return m_gizmo; }
        GizmoOptions &          getGizmoOptions     () { return m_gizmo; }

        bool                    showCullingInfo     () const { return m_cullingInfo;}

    private:
        renderer::GUITheme      m_guiTheme = renderer::GUITheme::ImGui_Dark;       
        GizmoOptions            m_gizmo;
        bool                    m_cullingInfo = false;
    };
}