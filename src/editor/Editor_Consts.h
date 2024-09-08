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
            inline static const ImVec2 SizeSmall = ImVec2(22, 22);
            inline static const ImVec2 SizeMedium = ImVec2(64, 0);
            inline static const ImVec2 SizeLarge = ImVec2(140, 0);
        }

        namespace font
        {
            inline static const float Height = 14.0f;
        }

        namespace label
        {
            inline static const float PixelWidth = 112.0f;
            inline static const core::uint TextLength = 14;
        }

        namespace range
        {
            inline static const float minFloat = FLT_MIN;
            inline static const float maxFloat = FLT_MAX;
           
            inline static const core::u32 minUInt = 0;
            inline static const core::u32 maxUInt = UINT_MAX;
            
            inline static const core::i32 minInt = INT_MIN;
            inline static const core::i32 maxInt = INT_MAX;
        }

        namespace draganddrop
        {
            inline static const ImVec2 interlineSize = ImVec2(256.0f, 8.0f);

            vg_enum(Type, core::u8,
                BeforeNode,
                Node,
                AfterNode
            );
        }

        // from https://fontawesome.com/v6/search?m=free
        namespace icon
        {
            inline static const char * About                = ICON_FA_CIRCLE_QUESTION;
            inline static const char * Animation            = ICON_FA_PERSON_RUNNING;
            inline static const char * Apply                = ICON_FA_BOLT;
            inline static const char * Camera               = ICON_FA_VIDEO;
            inline static const char * Canvas               = ICON_FA_CROP_SIMPLE;
            inline static const char * CharacterController  = ICON_FA_CAPSULES;
            inline static const char * Checked              = ICON_FA_SQUARE_CHECK;
            inline static const char * Console              = ICON_FA_TABLE_LIST;
            inline static const char * Clock                = ICON_FA_CLOCK;
            inline static const char * Destroy              = ICON_FA_CIRCLE_XMARK;
            inline static const char * Edit                 = ICON_FA_PEN_TO_SQUARE;
            inline static const char * Editor               = ICON_FA_SCREWDRIVER_WRENCH;
            inline static const char * EditorView           = ICON_FA_DESKTOP;
            inline static const char * Engine               = ICON_FA_GEAR;
            inline static const char * Error                = ICON_FA_BUG;
            inline static const char * File                 = ICON_FA_FILE;
            inline static const char * Font                 = ICON_FA_FONT;
            inline static const char * FPS                  = ICON_FA_STOPWATCH;
            inline static const char * Folder               = ICON_FA_FOLDER_OPEN;
            inline static const char * NewFolder            = ICON_FA_FOLDER_PLUS;
            inline static const char * GameView             = ICON_FA_DISPLAY;
            inline static const char * GameObject           = ICON_FA_SITEMAP;// ICON_FA_PERSON;
            inline static const char * Info                 = ICON_FA_CIRCLE_INFO;
            inline static const char * Input                = ICON_FA_GAMEPAD;
            inline static const char * Inspector            = ICON_FA_MAGNIFYING_GLASS;
            inline static const char * Image                = ICON_FA_IMAGE;
            inline static const char * Light                = ICON_FA_LIGHTBULB;
            inline static const char * Link                 = ICON_FA_LINK;
            inline static const char * Maximize             = ICON_FA_MAXIMIZE;
            inline static const char * Mesh                 = ICON_FA_CUBE;
            inline static const char * Minimize             = ICON_FA_MINIMIZE;
            inline static const char * Minus                = ICON_FA_SQUARE_MINUS;
            inline static const char * New                  = ICON_FA_FILE_MEDICAL;
            inline static const char * Override             = ICON_FA_STAR;
            inline static const char * Pause                = ICON_FA_PAUSE;
            inline static const char * Physics              = ICON_FA_CAPSULES;
            inline static const char * Platform             = ICON_FA_COMPUTER;
            inline static const char * Play                 = ICON_FA_PLAY;
            inline static const char * Plus                 = ICON_FA_SQUARE_PLUS;
            inline static const char * Plugin               = ICON_FA_PLUG;
            inline static const char * Prefab               = ICON_FA_PUZZLE_PIECE;
            inline static const char * Reload               = ICON_FA_ROTATE_RIGHT;
            inline static const char * Renderer             = ICON_FA_DISPLAY;
            inline static const char * Resource             = ICON_FA_DATABASE;
            inline static const char * Restart              = ICON_FA_CLOCK_ROTATE_LEFT;
            inline static const char * PhysicsBody          = ICON_FA_CUBES_STACKED;
            inline static const char * PhysicsShape         = ICON_FA_SHAPES;
            inline static const char * Rotate               = ICON_FA_ROTATE;
            inline static const char * Load                 = ICON_FA_FILE_EXPORT;
            inline static const char * Save                 = ICON_FA_FLOPPY_DISK;
            inline static const char * Scene                = ICON_FA_SITEMAP;
            inline static const char * Scale                = ICON_FA_ARROWS_LEFT_RIGHT_TO_LINE;
            inline static const char * Script               = ICON_FA_FILE_LINES;
            inline static const char * Shaders              = ICON_FA_HAT_WIZARD;
            inline static const char * Sliders              = ICON_FA_SLIDERS;
            inline static const char * Snap                 = ICON_FA_RULER_COMBINED;
            inline static const char * Statistics           = ICON_FA_CHART_SIMPLE;
            inline static const char * Stop                 = ICON_FA_STOP;
            inline static const char * Timer                = ICON_FA_STOPWATCH;
            inline static const char * Trashcan             = ICON_FA_TRASH_CAN;
            inline static const char * Translate            = ICON_FA_UP_DOWN_LEFT_RIGHT;
            inline static const char * Unchecked            = ICON_FA_SQUARE;
            inline static const char * Warning              = ICON_FA_TRIANGLE_EXCLAMATION;
            inline static const char * World                = ICON_FA_GLOBE;
        }
    }
}