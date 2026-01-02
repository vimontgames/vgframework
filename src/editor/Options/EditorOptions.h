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
    vg_enum_class(vg::editor, GizmoType, core::u8,
        Translate = 0,
        Rotate,
        Scale
    );

    vg_enum_class(vg::editor, GizmoSpace, core::u8,
        Local = 0,
        World
    );

    vg_enum_class(vg::editor, EditorDebugFlags, core::u64,
        Culling             = 0x0000000000000001,
        Inspector           = 0x0000000000000002,
        Picking             = 0x0000000000000004
    );

    vg_enum_class(vg::editor, PropertiesDisplayFlags, core::u64,
        Matrix              = 0x0000000000000001,   // Display properties with the 'PropertyFlag::DebugMatrix' flag (e.g. cached global matrix)
        Runtime             = 0x0000000000000002,   // Display properties with the 'PropertyFlag::DebugRuntime' flag (e.g. runtime flags)
        UID                 = 0x0000000000000004,   // Display properties with the 'PropertyFlag::DebugUID' flag (e.g. object UIDs)
        Misc                = 0x8000000000000000    // Display properties with the 'PropertyFlag::Debug' flag 
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

        bool                            Load                            (bool _async) final override;

        renderer::ICameraSettings *     GetCameraSettings               () const final override { return m_cameraSettings; }

        void                            SetPropertyValue                (const core::IProperty & _prop, void * _previousValue, void * _newValue) final override;

        bool                            setGizmoType                    (GizmoType _gizmoType);
        void                            setNextGizmo                    ();
        void                            setPreviousGizmo                ();

        bool                            setSnap                         (bool _enable);
        bool                            getSnap                         () const;


        const GizmoOptions &            getGizmoOptions                 () const { return m_gizmo; }
        GizmoOptions &                  getGizmoOptions                 () { return m_gizmo; }

        
        bool                            IsDebugCulling                  () const;
        bool                            IsDebugInspector                () const;
        bool                            IsDebugPicking                  () const;

        bool                            IsDebugMatrixPropertyVisible    () const;
        bool                            IsDebugRuntimePropertyVisible   () const;
        bool                            IsDebugUIDPropertyVisible       () const;
        bool                            IsMiscDebugPropertyVisible      () const;

        const ImGuiConsoleOptions &     getConsoleOptions               () const { return m_consoleOptions; }
        ImGuiConsoleOptions &           getConsoleOptions               () { return m_consoleOptions; }

    private:
        renderer::Theme                 m_theme = renderer::Theme::ImGui_Dark;
        GizmoOptions                    m_gizmo;
        EditorDebugFlags                m_editorDebugFlags = (EditorDebugFlags)0x0;
        PropertiesDisplayFlags          m_propertiesDisplayFlags = (PropertiesDisplayFlags)0x0;
        ImGuiConsoleOptions             m_consoleOptions;
        renderer::ICameraSettings *     m_cameraSettings = nullptr;          
    };
}