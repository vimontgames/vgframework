#pragma once
#include "core/Singleton/Singleton.h"
#include "editor/IEditorOptions.h"
#include "editor/ImGui/Window/Console/ImGuiConsoleOptions.h"
#include "renderer/IImGuiAdapter.h"

namespace vg::renderer
{
    class ICameraSettings;
}

namespace vg::editor
{
    vg_enum_class(GizmoType, core::u8,
        Translate = 0,
        Rotate,
        Scale
    );

    vg_enum_class(GizmoSpace, core::u8,
        Local = 0,
        World
    );

    vg_enum_class(EditorDebugFlags, core::u64,
        Culling     = 0x00000001,
        Inspector   = 0x00000002,
        Picking     = 0x00000004,
        Properties  = 0x00000008
    );

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
        ~EditorOptions();

        bool                            Load                    (bool _async) final override;

        renderer::ICameraSettings *     GetCameraSettings       () const final override { return m_cameraSettings; }

        void                            SetPropertyValue        (const core::IProperty & _prop, void * _previousValue, void * _newValue) final override;

        bool                            setGizmoType            (GizmoType _gizmoType);
        void                            setNextGizmo            ();
        void                            setPreviousGizmo        ();

        bool                            setSnap                 (bool _enable);
        bool                            getSnap                 () const;


        const GizmoOptions &            getGizmoOptions         () const { return m_gizmo; }
        GizmoOptions &                  getGizmoOptions         () { return m_gizmo; }

        
        bool                            IsDebugCulling          () const;
        bool                            IsDebugInspector        () const;
        bool                            IsDebugPicking          () const;
        bool                            IsDebugPropertyVisible  () const;

        const ImGuiConsoleOptions &     getConsoleOptions       () const { return m_consoleOptions; }
        ImGuiConsoleOptions &           getConsoleOptions       () { return m_consoleOptions; }

    private:
        renderer::Theme                 m_theme = renderer::Theme::ImGui_Dark;
        GizmoOptions                    m_gizmo;
        EditorDebugFlags                m_debugFlags = (EditorDebugFlags)0x0;
        ImGuiConsoleOptions             m_consoleOptions;
        renderer::ICameraSettings *     m_cameraSettings = nullptr;          
    };
}