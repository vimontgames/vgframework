#pragma once

#include "editor/ImGui/ImGui.h"
#include "IconFont/IconsFontAwesome6.h"

namespace vg::editor
{
    vg_enum(vg::editor, DragAndDropType, core::u8,
        BeforeNode,
        Node,
        AfterNode
    );

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
            inline static const float DefaultFontHeight = 14.0f;
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
        }

        // from https://fontawesome.com/v6/search?m=free
        namespace icon
        {
            inline constexpr char About                []= ICON_FA_CIRCLE_QUESTION;
            inline constexpr char Animation            []= ICON_FA_PERSON_RUNNING;
            inline constexpr char Apply                []= ICON_FA_BOLT;
            inline constexpr char Camera               []= ICON_FA_VIDEO;
            inline constexpr char Canvas               []= ICON_FA_CROP_SIMPLE;
            inline constexpr char CharacterController  []= ICON_FA_CAPSULES;
            inline constexpr char Checked              []= ICON_FA_SQUARE_CHECK;
            inline constexpr char Console              []= ICON_FA_TABLE_LIST;
            inline constexpr char Clock                []= ICON_FA_CLOCK;
            inline constexpr char Curve                []= ICON_FA_CHART_LINE;
            inline constexpr char Destroy              []= ICON_FA_CIRCLE_XMARK;
            inline constexpr char Edit                 []= ICON_FA_PEN_TO_SQUARE;
            inline constexpr char Editor               []= ICON_FA_SCREWDRIVER_WRENCH;
            inline constexpr char EditorView           []= ICON_FA_DESKTOP;
            inline constexpr char Engine               []= ICON_FA_GEAR;
            inline constexpr char Error                []= ICON_FA_BUG;
            inline constexpr char Environment          []= ICON_FA_CLOUD_MOON;
            inline constexpr char FLipBook             []= ICON_FA_FILM;
            inline constexpr char File                 []= ICON_FA_FILE;
            inline constexpr char Fire                 []= ICON_FA_FIRE;
            inline constexpr char FitVertical          []= ICON_FA_ARROWS_UP_DOWN;
            inline constexpr char Font                 []= ICON_FA_FONT;
            inline constexpr char FPS                  []= ICON_FA_STOPWATCH;
            inline constexpr char Folder               []= ICON_FA_FOLDER_OPEN;
            inline constexpr char Game                 []= ICON_FA_GAMEPAD;
            inline constexpr char NewFolder            []= ICON_FA_FOLDER_PLUS;
            inline constexpr char GameView             []= ICON_FA_DISPLAY;
            inline constexpr char GameObject           []= ICON_FA_PERSON;
            inline constexpr char Info                 []= ICON_FA_CIRCLE_INFO;
            inline constexpr char Input                []= ICON_FA_KEYBOARD;
            inline constexpr char Inspector            []= ICON_FA_MAGNIFYING_GLASS;
            inline constexpr char Image                []= ICON_FA_IMAGE;
            inline constexpr char Light                []= ICON_FA_LIGHTBULB;
            inline constexpr char Link                 []= ICON_FA_LINK;
            inline constexpr char Maximize             []= ICON_FA_MAXIMIZE;
            inline constexpr char Mesh                 []= ICON_FA_CUBE;
            inline constexpr char Minimize             []= ICON_FA_MINIMIZE;
            inline constexpr char Minus                []= ICON_FA_SQUARE_MINUS;
            inline constexpr char MoveUp               []= ICON_FA_CIRCLE_UP;
            inline constexpr char MoveDown             []= ICON_FA_CIRCLE_DOWN;
            inline constexpr char New                  []= ICON_FA_FILE_MEDICAL;
            inline constexpr char Parent               []= ICON_FA_PERSON_BREASTFEEDING;
            inline constexpr char Override             []= ICON_FA_STAR;
            inline constexpr char Pause                []= ICON_FA_PAUSE;
            inline constexpr char Physics              []= ICON_FA_CAPSULES;
            inline constexpr char Platform             []= ICON_FA_COMPUTER;
            inline constexpr char Play                 []= ICON_FA_PLAY;
            inline constexpr char Plus                 []= ICON_FA_SQUARE_PLUS;
            inline constexpr char Plugin               []= ICON_FA_PLUG;
            inline constexpr char Prefab               []= ICON_FA_PUZZLE_PIECE;
            inline constexpr char ResetRange           []= ICON_FA_VECTOR_SQUARE;
            inline constexpr char Reload               []= ICON_FA_ROTATE_RIGHT;
            inline constexpr char Renderer             []= ICON_FA_DISPLAY;
            inline constexpr char Resource             []= ICON_FA_DATABASE;
            inline constexpr char Restart              []= ICON_FA_CLOCK_ROTATE_LEFT;
            inline constexpr char PhysicsBody          []= ICON_FA_CUBES_STACKED;
            inline constexpr char PhysicsShape         []= ICON_FA_SHAPES;
            inline constexpr char PhysicsMergeStatic   []= ICON_FA_ARROWS_TO_DOT;
            inline constexpr char Rotate               []= ICON_FA_ROTATE;
            inline constexpr char Load                 []= ICON_FA_FILE_EXPORT;
            inline constexpr char Material             []= ICON_FA_PAINTBRUSH;
            inline constexpr char Save                 []= ICON_FA_FLOPPY_DISK;
            inline constexpr char Scene                []= ICON_FA_SITEMAP;
            inline constexpr char Scale                []= ICON_FA_ARROWS_LEFT_RIGHT_TO_LINE;
            inline constexpr char Script               []= ICON_FA_FILE_LINES;
            inline constexpr char Shaders              []= ICON_FA_HAT_WIZARD;
            inline constexpr char Sliders              []= ICON_FA_SLIDERS;
            inline constexpr char Sound                []= ICON_FA_VOLUME_HIGH;
            inline constexpr char Snap                 []= ICON_FA_RULER_COMBINED;
            inline constexpr char Statistics           []= ICON_FA_CHART_SIMPLE;
            inline constexpr char Stop                 []= ICON_FA_STOP;
            inline constexpr char Timer                []= ICON_FA_STOPWATCH;
            inline constexpr char Trashcan             []= ICON_FA_TRASH_CAN;
            inline constexpr char Translate            []= ICON_FA_UP_DOWN_LEFT_RIGHT;
            inline constexpr char Unchecked            []= ICON_FA_SQUARE;
            inline constexpr char Undo                 []= ICON_FA_ROTATE_LEFT;
            inline constexpr char VehicleController    []= ICON_FA_CAR;
            inline constexpr char Warning              []= ICON_FA_TRIANGLE_EXCLAMATION;
            inline constexpr char Wheel                []= ICON_FA_CIRCLE;
            inline constexpr char World                []= ICON_FA_GLOBE;
        }

        namespace label
        {
            inline constexpr char Transform            [] = ICON_FA_SITEMAP " Transform";

            namespace physics
            {
                inline constexpr char Wheels           [] = ICON_FA_CIRCLE " Wheels";
            }

            namespace particle
            {
                inline constexpr char Emitters         [] = ICON_FA_BOLT " Emitters";

                inline constexpr char Life             [] = ICON_FA_HEART_PULSE " Life";
                inline constexpr char Size             [] = ICON_FA_MAXIMIZE " Size";
                inline constexpr char Color            [] = ICON_FA_DROPLET " Color"; // ICON_FA_PALETTE " Color";
                inline constexpr char Opacity          [] = ICON_FA_CIRCLE_HALF_STROKE " Opacity";
                inline constexpr char Speed            [] = ICON_FA_GAUGE_SIMPLE_HIGH " Speed";
                inline constexpr char FlipBook         [] = ICON_FA_FILM " FlipBook";
                inline constexpr char Material         [] = ICON_FA_PAINTBRUSH " Material";
            }

            inline constexpr char Materials            [] = ICON_FA_PAINTBRUSH " Materials"; 
        }
    }
}