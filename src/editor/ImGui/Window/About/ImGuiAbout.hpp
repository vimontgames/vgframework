#include "ImguiAbout.h"
#include "renderer/ImGui_consts.h"
#include "version.h"

namespace vg::editor
{
    struct LibraryDescription
    {
        const char * name;
        const char * version;
        const char * url;
    };

    static const float columnWidth[3] = { 220, 150, 750 };

    //--------------------------------------------------------------------------------------
    ImGuiAbout::ImGuiAbout() :
        ImGuiWindow(style::icon::About, "", "About", ImGuiWindow::None)
    {
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAbout::drawLibraryDescriptionList(const char * _label, LibraryDescription * _infos, core::uint _count)
    {
        Columns(1);
        ImGui::PushStyle(renderer::FontStyle::Bold);
        Text(_label);
        Text("");
        ImGui::PopStyle();

        Columns(3, _label, false);

        SetColumnWidth(0, columnWidth[0]);
        SetColumnWidth(1, columnWidth[1]);
        SetColumnWidth(2, columnWidth[2]);

        for (uint i = 0; i < _count; ++i)
            Text(_infos[i].name);

        NextColumn();

        for (uint i = 0; i < _count; ++i)
            Text(_infos[i].version);

        NextColumn();

        for (uint i = 0; i < _count; ++i)
            textURL(_infos[i].url + strlen("https://"), _infos[i].url);

        Text("");

        Separator();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAbout::DrawGUI()
    {
        if (IconBegin(style::icon::About, "About", &m_isVisible, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse))
        {
            ImGui::PushDefaultFont();

            ImGui::PushStyle(renderer::FontStyle::Bold);
            Text("VG Framework %u.%u.%u", VG_FRAMEWORK_VERSION_MAJOR, VG_FRAMEWORK_VERSION_MINOR, VG_FRAMEWORK_VERSION_PATCH);
            Text("");
            ImGui::PopStyle();

            Columns(3, "author", false);

            SetColumnWidth(0, columnWidth[0]);
            SetColumnWidth(1, columnWidth[1]);
            SetColumnWidth(2, columnWidth[2]);

            Text("GitHub");
            Text("Mastodon");
            Text("");

            NextColumn();
            NextColumn();

            textURL("github.com/vimontgames/vgframework",       "https://github.com/vimontgames/vgframework");
            textURL("@benoitvimont@mastodon.gamedev.place",     "https://mastodon.gamedev.place/@benoitvimont");
            Text("");

            Separator();

            LibraryDescription SDKs[] =
            {
                { "Windows 10 SDK",         "10.0.17763.0",     "https://developer.microsoft.com/fr-fr/windows/downloads/sdk-archive/" },
                { "Vulkan SDK",             "1.3.290.0",        "https://vulkan.lunarg.com/sdk/home" },
            };

            drawLibraryDescriptionList("SDK", SDKs, (uint)countof(SDKs));

            LibraryDescription externLibs[] =
            {
                { "D3D12 Memory Allocator", "",                 "https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator" },
                { "D3D12 HDR Sample",       "",                 "https://github.com/microsoft/directx-graphics-samples/tree/master/Samples/Desktop/D3D12HDR" },
                { "Dear ImGui - Docking",   "1.91.2",           "https://github.com/ocornut/imgui/tree/docking" },
                { "Dirent",                 "1.23.2",           "https://github.com/tronkko/dirent" },
                { "DirectXShaderCompiler",  "v1.7.2308",        "https://github.com/microsoft/DirectXShaderCompiler/releases/tag/v1.7.2308" },
                { "Font-Awesome",           "6.x",              "https://github.com/FortAwesome/Font-Awesome/tree/6.x" },
                { "fmt",                    "10.1.1",           "https://github.com/fmtlib/fmt/releases/tag/10.1.1" },
                { "FXAA",                   "3.11",             "https://github.com/GameTechDev/CMAA2/blob/master/Projects/CMAA2/FXAA/Fxaa3_11.h" },
                { "hlslpp",                 "3.5.2",            "https://github.com/redorav/hlslpp/releases/tag/3.5.2" },
                { "IconFontCppHeaders",     "90da802",          "https://github.com/juliettef/IconFontCppHeaders" },
                { "ImGuiFileDialog",        "0.6.7",            "https://github.com/aiekick/ImGuiFileDialog" },
                { "ImGuizmo",               "1.83",             "https://github.com/CedricGuillemet/ImGuizmo/releases/tag/1.83" },
                { "JoltPhysics",            "5.0.0",            "https://github.com/jrouwe/JoltPhysics/releases/tag/v5.0.0" },
                { "meta_enum_lite",         "1.0",              "https://github.com/vimontgames/meta_enum_lite" },
                { "optick",                 "1.3.1",            "https://github.com/bombomby/optick" },
                { "px_sched",               "",                 "https://github.com/pplux/px" },
                { "SoLoud",                 "August 2024",      "https://github.com/jarikomppa/soloud" },
                { "stb_image",              "2.30",             "https://github.com/nothings/stb" },
                { "tinyXML2",               "10.0.0",           "https://github.com/leethomason/tinyxml2" },
                { "UFBX",                   "0.14.3",           "https://github.com/ufbx/ufbx" },
                { "VulkanMemoryAllocator",  "3.0.1",            "https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator" },
                { "WinPixEventRuntime",     "1.0.231030001",    "https://www.nuget.org/packages/WinPixEventRuntime" },
            };

            drawLibraryDescriptionList("Extern libs", externLibs, (uint)countof(externLibs));

            LibraryDescription fonts[] =
            {
                { "Font-Awesome",   "6.x",  "https://github.com/FortAwesome/Font-Awesome/tree/6.x" },
                { "Rowdies",        "",     "https://github.com/magictype/rowdy" },
                { "RubikMonoOne",   "",     "https://fonts.google.com/specimen/Rubik+Mono+One" },
                { "UbuntuMono",     "1.0",  "https://fonts.google.com/specimen/Ubuntu+Mono" }
            };

            drawLibraryDescriptionList("Fonts", fonts, (uint)countof(fonts));

            LibraryDescription models[] =
            {
                { "3D Football",          "prabath-weerapana",  "https://www.cgtrader.com/free-3d-models/sports/game/3d-foot-ball" },
                { "Baseball Bat",         "sepandjahrominejad", "https://www.cgtrader.com/free-3d-models/military/melee/baseball-bat-5c0d0b8c-0ff9-40f8-b49d-779978a2801b" },
                { "FootBall american",    "orsbone",            "https://www.cgtrader.com/free-3d-models/sports/equipment/football-91c641b9-d246-4d71-a31c-d687762dd94f" },
                { "Katana samurai sword", "kizaru",             "https://www.cgtrader.com/free-3d-models/military/melee/katana-low-poly-59c307a4-5eaf-480d-b2a3-841d8d8c7240" },
                { "Luigi",                "Anthony Yanez",      "https://www.turbosquid.com/3d-models/luigi-super-model-1308288" },
                { "Traffic Cone",         "hinndia",            "https://www.cgtrader.com/free-3d-models/exterior/street-exterior/traffic-cone-5849a434-2331-4a90-9b87-087ccc16cd0c" },
                { "Soccergoal",           "NorbertVarga",       "https://www.turbosquid.com/3d-models/soccergoal-3d-model-1840894" }
            };

            drawLibraryDescriptionList("Models", models, (uint)countof(models));

            LibraryDescription environmentMaps[] =
            {
                { "Free HDR Map 779",          "hdri-skies.com",  "https://hdri-skies.com/free-hdris/hdr-map-779/" }
            };

            LibraryDescription sound[] =
            {
                { "FX - Swoosh - normal", "bolkmar",              "https://freesound.org/people/bolkmar/sounds/451127/" },
                { "Large sword swing",    "Luis0413",             "https://freesound.org/people/Luis0413/sounds/737748/" },
                { "Tally Dub",            "Brotheration Records", "https://pixabay.com/fr/music/reggae-tally-dub-brotheration-records-2016-140287/" }
            };

            drawLibraryDescriptionList("Sound", sound, (uint)countof(sound));

            Columns(1);
            ImGui::PushStyle(renderer::FontStyle::Bold);
            Text("Special thanks");
            Text("");
            ImGui::PopStyle();
            Text("JOYxt, Onduril, SlavSquat, Bob, Guigui, Marcel, Hamilcar and all the old guard.");

            ImGui::PopFont();
            End();            
        }
    }
}