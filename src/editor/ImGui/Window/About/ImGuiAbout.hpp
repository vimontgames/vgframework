#include "ImguiAbout.h"

namespace vg::editor
{
    struct LibraryDescription
    {
        const char * name;
        const char * version;
        const char * url;
    };

    static const float columnWidth[3] = { 256, 128, 640 };

    //--------------------------------------------------------------------------------------
    ImGuiAbout::ImGuiAbout() :
        ImGuiWindow(style::icon::About, "", "About", ImGuiWindow::None)
    {
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAbout::drawLibraryDescriptionList(const char * _label, LibraryDescription * _infos, core::uint _count)
    {
        Columns(1);
        Text(_label);
        Text("");

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
            Text("VG Framework");
            Text("");

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
                { "Vulkan SDK",             "1.3.261.1",        "https://sdk.lunarg.com/sdk/download/1.3.261.1/windows/VulkanSDK-1.3.261.1-Installer.exe" },
                { "Windows 10 SDK",         "10.0.17763.0",     "https://developer.microsoft.com/fr-fr/windows/downloads/sdk-archive/" },
            };

            drawLibraryDescriptionList("SDK", SDKs, (uint)countof(SDKs));

            LibraryDescription thirdParties[] =
            {
                { "D3D12MemoryAllocator",   "",                 "https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator" },
                { "Dear ImGui",             "2a6d7b1",          "https://github.com/ocornut/imgui/tree/docking" },
                { "Dirent",                 "1.23.2",           "https://github.com/tronkko/dirent" },
                { "DirectXShaderCompiler",  "v1.7.2308",        "https://github.com/microsoft/DirectXShaderCompiler/releases/tag/v1.7.2308" },
                { "Font-Awesome",           "6.x",              "https://github.com/FortAwesome/Font-Awesome/tree/6.x" },
                { "fmt",                    "10.1.1",           "https://github.com/fmtlib/fmt/releases/tag/10.1.1" },
                { "hlslpp",                 "3.4",              "https://github.com/redorav/hlslpp/releases/tag/3.3.1" },
                { "IconFontCppHeaders",     "90da802",          "https://github.com/juliettef/IconFontCppHeaders" },
                { "ImGuiFileDialog",        "0.6.7",            "https://github.com/aiekick/ImGuiFileDialog" },
                { "ImGuizmo",               "1.83",             "https://github.com/CedricGuillemet/ImGuizmo/releases/tag/1.83" },
                { "JoltPhysics",            "4.0.1",            "https://github.com/jrouwe/JoltPhysics/releases/tag/v4.0.1" },
                { "magic_enum",             "0.9.4",            "https://github.com/Neargye/magic_enum/releases/tag/v0.9.4" },
                { "optick",                 "1.3.1",            "https://github.com/bombomby/optick" },
                { "px_sched",               "",                 "https://github.com/pplux/px" },
                { "stb_image",              "",                 "https://github.com/nothings/stb" },
                { "tinyXML2",               "10.0.0",           "https://github.com/leethomason/tinyxml2" },
                { "UFBX",                   "0.5.0-30f035b",    "https://github.com/ufbx/ufbx" },
                { "VulkanMemoryAllocator",  "3.0.1",            "https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator" },
                { "WinPixEventRuntime",     "1.0.231030001",    "https://www.nuget.org/packages/WinPixEventRuntime" },
            };

            drawLibraryDescriptionList("3rd-parties:", thirdParties, (uint)countof(thirdParties));

            Columns(1);
            Text("Special Thanks to SlavSquat, Bob, Guigui, Marcel, Hamilcar and the old guard");

            End();
        }
    }
}