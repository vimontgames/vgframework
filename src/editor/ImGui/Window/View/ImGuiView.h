#pragma once

#include "editor/ImGui/Window/ImGuiWindow.h"
#include "gfx/IView.h"

namespace vg::editor
{
    class ImGuiView : public ImGuiWindow
    {
    public:
                            ImGuiView           (const char * _icon, const core::string & _path, const core::string & _name, Flags _flags, gfx::ViewTarget _target, core::uint _index);
                            ~ImGuiView          ();

        void                updateEditorCamera  (float _dt);

        void                DrawGUI             () final override;

    protected:
        bool                drawGizmo           ();
        void                updatePicking       (bool & _showTooltip, core::string & _tooltipMsg);

    private:
        gfx::ViewTarget     m_target            = gfx::ViewTarget::Game;
        core::uint          m_index             = -1;
        core::uint2         m_size              = core::uint2(0, 0);
        gfx::IView *        m_view              = nullptr;
        gfx::ITexture *     m_texture           = nullptr;
        bool                m_manipulating      = false;

        struct EditorCamera
        {
            EditorCamera();

            core::float4x4      m_matrix;

            float               m_fovY;

            float               m_near;
            float               m_far;

            float               m_pitch;
            float               m_yaw;
            float               m_roll;

            float               m_moveSpeed;
            float               m_rotSpeed;
            float               m_panXYSpeed;
            float               m_zoomSpeed;

            bool                m_panning;
            core::float4        m_panWorldPosStart;
            core::uint2         m_pan2DPosStart;
        };
        EditorCamera        m_editorCam;
    };
}