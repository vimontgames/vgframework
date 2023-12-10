#pragma once

#include "editor/ImGui/ImGui.h"
#include "IconFont/IconsFontAwesome6.h"

namespace vg::editor
{
    namespace style
    {
        namespace dialog
        {
            inline static const ImVec2 Size = ImVec2(860, 400);
        }

        namespace button
        {
            inline static const ImVec2 SizeSmall = ImVec2(23, 23);
            inline static const ImVec2 SizeMedium = ImVec2(64, 0);
        }

        namespace label
        {
            inline static const float PixelWidth = 96.0f;
            inline static const core::uint TextLength = 14;
        }

        namespace draganddrop
        {
            inline static const ImVec2 interlineSize = ImVec2(256.0f, 8.0f);

            enum Type
            {
                BeforeNode,
                Node,
                AfterNode
            };
        }

        // from https://fontawesome.com/v6/search?m=free
        namespace icon
        {
            inline static const char * About        = ICON_FA_CIRCLE_QUESTION;
            inline static const char * Renderer     = ICON_FA_DISPLAY;
            inline static const char * Inspector    = ICON_FA_MAGNIFYING_GLASS;
            inline static const char * Platform     = ICON_FA_COMPUTER;
            inline static const char * Shaders      = ICON_FA_HAT_WIZARD;
            inline static const char * FPS          = ICON_FA_STOPWATCH;
            inline static const char * Scene        = ICON_FA_MAP;
            inline static const char * Resource     = ICON_FA_DATABASE;
            inline static const char * EditorView   = ICON_FA_DESKTOP;
            inline static const char * GameView     = ICON_FA_DISPLAY;
            inline static const char * Play         = ICON_FA_PLAY;
            inline static const char * Pause        = ICON_FA_PAUSE;
            inline static const char * Stop         = ICON_FA_STOP;
            inline static const char * Plugin       = ICON_FA_PLUG;
            inline static const char * Console      = ICON_FA_TABLE_LIST;
            inline static const char * Info         = ICON_FA_CIRCLE_INFO;
            inline static const char * Warning      = ICON_FA_TRIANGLE_EXCLAMATION;
            inline static const char * Error        = ICON_FA_BUG;
            inline static const char * Minimize     = ICON_FA_MINIMIZE;
            inline static const char * Maximize     = ICON_FA_MAXIMIZE;
            inline static const char * Input        = ICON_FA_GAMEPAD;
            inline static const char * File         = ICON_FA_FILE;
            inline static const char * Plus         = ICON_FA_SQUARE_PLUS;
            inline static const char * Minus        = ICON_FA_SQUARE_MINUS;
            inline static const char * Checked      = ICON_FA_SQUARE_CHECK;
            inline static const char * Unchecked    = ICON_FA_SQUARE;
            inline static const char * Trashcan     = ICON_FA_TRASH_CAN;
            inline static const char * Physics      = ICON_FA_CAPSULES;
            inline static const char * Editor       = ICON_FA_SCREWDRIVER_WRENCH;
            inline static const char * Restart      = ICON_FA_CLOCK_ROTATE_LEFT;
        }
    }
}