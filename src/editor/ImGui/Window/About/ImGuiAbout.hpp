#include "ImguiAbout.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    void ImGuiAbout::DrawGUI()
    {
        if (IconBegin(style::icon::About, "About", &m_isVisible, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse))
        {
            float columnWidth[3] = { 256, 128, 620 };

            const auto & engine = Editor::get()->getEngine();
            const auto engineVersion = engine->GetVersion();
            const auto rendererVersion = engine->GetRenderer()->GetVersion();

            Text("VG Framework Engine %u.%u Renderer %u.%u", engineVersion.major, engineVersion.minor, rendererVersion.major, rendererVersion.minor);
            Text("");

            Columns(3, "author", false);

            SetColumnWidth(0, columnWidth[0]);
            SetColumnWidth(1, columnWidth[1]);
            SetColumnWidth(2, columnWidth[2]);

            Text("GitHub");
            Text("Twitter");
            Text("Mastodon");
            Text("");

            NextColumn();
            NextColumn();

            textURL("github.com/vimontgames/vgframework", "https://github.com/vimontgames/vgframework");
            textURL("@benoitbimont", "https://twitter.com/benoitvimont");
            textURL("@benoitvimont@mastodon.gamedev.place", "https://mastodon.gamedev.place/@benoitvimont");
            Text("");

            Separator();

            Columns(1);
            Text("SDKs:");
            Text("");

            Columns(3, "SDK", false);

            SetColumnWidth(0, columnWidth[0]);
            SetColumnWidth(1, columnWidth[1]);
            SetColumnWidth(2, columnWidth[2]);

            // name                        
            Text("FBX SDK");
            Text("Vulkan SDK");
            Text("Windows 10 SDK");

            Text("");
            NextColumn();

            // version
            Text("2020.0.1");
            Text("1.2.148.0");
            Text("10.0.17763.0");

            Text("");
            NextColumn();
            // url
            textURL("www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2020-0", "https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2020-0");
            textURL("vulkan.lunarg.com/sdk/home", "https://vulkan.lunarg.com/sdk/home");
            textURL("developer.microsoft.com/fr-fr/windows/downloads/sdk-archive", "https://developer.microsoft.com/fr-fr/windows/downloads/sdk-archive/");
            Text("");

            Separator();

            Columns(1);
            Text("3rd-parties:");
            Text("");

            Columns(3, "3rdparties", false);

            SetColumnWidth(0, columnWidth[0]);
            SetColumnWidth(1, columnWidth[1]);
            SetColumnWidth(2, columnWidth[2]);

            struct ThirdPartyInfo
            {
                const char * name;
                const char * version;
                const char * url;
            };

            ThirdPartyInfo thirdParties[] =
            {
                { "D3D12MemoryAllocator",   "",                 "https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator" },
                { "Dear ImGui",             "8cbd391",          "https://github.com/ocornut/imgui/tree/docking" },
                { "Dirent",                 "833b692",          "https://github.com/tronkko/dirent" },
                { "Font-Awesome",           "6.x ",             "https://github.com/FortAwesome/Font-Awesome/tree/6.x" },
                { "hlslpp",                 "",                 "https://github.com/redorav/hlslpp" },
                { "IconFontCppHeaders",     "90da802",          "https://github.com/juliettef/IconFontCppHeaders" },
                { "ImGui-Addons",           "ea0af59",          "https://github.com/gallickgunner/ImGui-Addons" },
                { "magic_enum",             "",                 "https://github.com/Neargye/magic_enum" },
                { "optick",                 "1.3.1",            "https://github.com/bombomby/optick" },
                { "px_sched",               "",                 "https://github.com/pplux/px" },
                { "stb_image",              "",                 "https://github.com/nothings/stb" },
                { "tinyXML2",               "e45d9d1",          "https://github.com/leethomason/tinyxml2" },
                { "VulkanMemoryAllocator",  "",                 "https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator" },
                { "WinPixEventRuntime",     "1.0.200127001",    "https://www.nuget.org/packages/WinPixEventRuntime" },
            };


            for (uint i = 0; i < countof(thirdParties); ++i)
                Text(thirdParties[i].name);

            NextColumn();

            for (uint i = 0; i < countof(thirdParties); ++i)
                Text(thirdParties[i].version);

            NextColumn();

            for (uint i = 0; i < countof(thirdParties); ++i)
                textURL(thirdParties[i].url + strlen("https://"), thirdParties[i].url);

            Text("");

            Separator();

            Columns(1);
            Text("Special Thanks:");
            Text("");
            Text("To Bob, Guigui, Marcel, Hamilcar, and SlavSquat.");

            End();
        }
    }
}