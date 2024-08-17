#pragma once

#include "editor/ImGui/Window/ImGuiWindow.h"
#include "gfx/IView.h"

namespace vg::core
{
    class IWorld;
}

namespace vg::editor
{
    class ImGuiView : public ImGuiWindow
    {
    public:
                                        ImGuiView           (const char * _icon, const core::string & _path, const core::string & _name, Flags _flags, gfx::ViewTarget _target, core::uint _index = -1);
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

        virtual gfx::IView::Flags       GetViewFlags        () const; 
        virtual core::IWorld*           GetWorld            () const;;
        virtual void                    OnCloseWindow       ();
        virtual bool                    UpdateScene         () { return true; };

        const gfx::ViewTarget &         GetViewTarget       () const { return m_target;}
        gfx::ViewIndex                  GetViewIndex        () const { return m_index; }

        void                            setWindowFocus      ();

        void                            focus               (core::IGameObject * _gameObject);
        void                            focus               (const core::vector<core::IGameObject * > & _gameObjects);

    protected:
        bool                            drawGizmo           ();
        void                            updatePicking       (bool & _showTooltip, core::string & _tooltipMsg);

    protected:
        gfx::ViewTarget                 m_target            = gfx::ViewTarget::Game;
        gfx::ViewIndex                  m_index             = gfx::ViewIndexInvalid;
        core::uint2                     m_size              = core::uint2(0, 0);
        gfx::IView *                    m_view              = nullptr;
        gfx::ITexture *                 m_texture           = nullptr;
        bool                            m_manipulating      = false;
        bool                            m_closeNextFrame    = false;

        struct EditorCamera
        {
            EditorCamera();

            core::float4x4          m_matrix;

            float                   m_fovY;

            float                   m_near;
            float                   m_far;

            float                   m_moveSpeed;
            float                   m_rotSpeed;
            float                   m_panXYSpeed;
            float                   m_zoomSpeed;

            bool                    m_panning;
            core::float4            m_panWorldPosStart;
            core::uint2             m_pan2DPosStart;
        };
        EditorCamera            m_editorCam;
    };
}