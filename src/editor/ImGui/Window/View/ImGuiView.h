#pragma once

#include "gfx/IFrameGraphViewport.h"
#include "editor/ImGui/Window/ImGuiWindow.h"

namespace vg::core
{
    class IWorld;
}

namespace vg::renderer
{
    class IViewport;
    class IView;
    class ICameraSettings;
    enum class ViewFlags : core::u32;
}

namespace vg::editor
{
    class ImGuiView : public ImGuiWindow
    {
    public:
                                        ImGuiView           (const char * _icon, const core::string & _path, const core::string & _name, Flags _flags, gfx::ViewportTarget _target, core::uint _viewportIndex = -1);
                                        ~ImGuiView          ();

        void                            updateEditorCamera  (float _dt);

        core::string                    GetTitle            () const override;
        void                            DrawGUI             () final override;

        virtual bool                    ShowToolbar         () const { return false; }
        virtual void                    DrawToolbar         () {};

        virtual bool                    ShowContextMenu     () const { return false; }
        virtual ImGuiMenu::Status       DrawContextMenu     () { return ImGuiMenu::Status::None; };

        virtual bool                    ShowTitlebarMenu    () const { return false;};
        virtual void                    DrawTitlebarMenu    () {};

        virtual renderer::ViewFlags     GetViewFlags        () const; 
        virtual core::IWorld *          GetWorld            () const;
        virtual void                    OnCloseWindow       ();
        virtual bool                    UpdateScene         () { return true; };

        gfx::ViewportTarget             getViewportTarget   () const;
        gfx::ViewportIndex              getViewportIndex    () const;

        void                            setWindowFocus      ();

        void                            focus               (core::IGameObject * _gameObject);
        void                            focus               (const core::vector<core::IGameObject * > & _gameObjects);

    protected:
        bool                            drawGizmo           (const renderer::IView * _view);
        void                            updatePicking       (bool & _showTooltip, core::string & _tooltipMsg);

    protected:
        gfx::ViewportTarget             m_target            = gfx::ViewportTarget::Game;
        core::uint2                     m_size              = core::uint2(0, 0);
        renderer::IViewport *           m_viewport          = nullptr;
        gfx::ViewportIndex              m_viewportIndex     = (gfx::ViewportIndex)-1;
        gfx::ITexture *                 m_texture           = nullptr;
        bool                            m_manipulating      = false;
        bool                            m_closeNextFrame    = false;

        struct EditorCamera
        {
            EditorCamera();
            ~EditorCamera();

            core::float4x4                      m_matrix;
            float                               m_moveSpeed;
            float                               m_rotSpeed;
            float                               m_panXYSpeed;
            float                               m_zoomSpeed;

            bool                                m_panning;
            core::float4                        m_panWorldPosStart;
            core::uint2                         m_pan2DPosStart;
        };
        EditorCamera            m_editorCam;

        core::uint              m_selectedObjectCount      = 0;
        core::float3            m_maxTranslationSnap       = core::float3(0,0,0);
        float                   m_maxRotationSnapInDegrees = 0;
        float                   m_maxScaleSnap             = 0;
    };
}