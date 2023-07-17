#pragma once

#include "IconFont/IconsFontAwesome6.h"
#include "graphics/renderer/Imgui/imguiExtensions.h"

namespace vg::graphics::renderer
{
    namespace Editor
    {
        inline static const ImVec2 ButtonSize = ImVec2(0,0);
        inline static const ImVec2 LoadSaveDialogSize = ImVec2(860, 400);

        namespace Icon
        {
            // https://fontawesome.com/v6/search?m=free
            inline static const char * About        = ICON_FA_CIRCLE_QUESTION;
            inline static const char * Display      = ICON_FA_DISPLAY;
            inline static const char * Inspector    = ICON_FA_MAGNIFYING_GLASS;
            inline static const char * Platform     = ICON_FA_COMPUTER;
            inline static const char * Shaders      = ICON_FA_HAT_WIZARD;
            inline static const char * FPS          = ICON_FA_STOPWATCH;
            inline static const char * Scene        = ICON_FA_MAP;
            inline static const char * Resource     = ICON_FA_DATABASE;
            inline static const char * EditorView   = ICON_FA_PEN_TO_SQUARE;
            inline static const char * GameView     = ICON_FA_GAMEPAD;
            inline static const char * Play         = ICON_FA_PLAY;
            inline static const char * Pause        = ICON_FA_PAUSE;
            inline static const char * Stop         = ICON_FA_STOP;
        }
    }
}