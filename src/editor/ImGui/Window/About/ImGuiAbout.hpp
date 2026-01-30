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

    static const float columnWidth[2] = { 256, 128 };

    //--------------------------------------------------------------------------------------
    ImGuiAbout::ImGuiAbout() :
        ImGuiWindow(style::icon::About, "", "About", ImGuiWindow::None)
    {
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAbout::drawLibraryDescriptionList(const char * _label, const char * _column0Label, const char * _column1Label, LibraryDescription * _infos, core::uint _count)
    {
        ImGui::PushStyle(renderer::FontStyle::Bold);
        ImGui::Text("%s", _label); // Display the label before the table
        ImGui::PopStyle();

        if (ImGui::BeginTable(_label, 2, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn(_column0Label, ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn(_column1Label, ImGuiTableColumnFlags_WidthFixed, 256);

            ImGui::TableHeadersRow(); 
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);

            for (uint i = 0; i < _count; ++i)
                textURL(_infos[i].name, _infos[i].url);

            ImGui::TableSetColumnIndex(1);

            for (uint i = 0; i < _count; ++i)
                ImGui::Text("%s", _infos[i].version);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::EndTable();
        }
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAbout::DrawGUI()
    {
        if (IconBegin(style::icon::About, "About", &m_isVisible, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse))
        {
            ImGui::PushDefaultFont();

            ImGui::PushStyle(renderer::FontStyle::Bold);
            textURL(fmt::sprintf("VG Framework %u.%u.%u", VG_FRAMEWORK_VERSION_MAJOR, VG_FRAMEWORK_VERSION_MINOR, VG_FRAMEWORK_VERSION_PATCH).c_str(), "https://github.com/vimontgames/vgframework");
            Text("");
            ImGui::PopStyle();
            
            LibraryDescription contributors[] =
            {
                { "Benualdo",             "GPU whisperer",        "https://github.com/Benualdo" },
                { "Joyxt",                "FBX exorcist",         "https://github.com/Joyxt" },
                { "kdridi",               "Linux evangelist",     "https://github.com/kdridi" },
                { "Onduril",              "ARM enjoyer",          "https://github.com/Onduril" }
            };

            drawLibraryDescriptionList("Contributors", "Name", "Role", contributors, (uint)countof(contributors));

            Separator();

            LibraryDescription SDKs[] =
            {
                { "Windows 10 SDK",         "10.0.26100.0",     "https://developer.microsoft.com/fr-fr/windows/downloads/sdk-archive/" },
                { "Vulkan SDK",             "1.3.290.0",        "https://vulkan.lunarg.com/sdk/home" },
            };

            drawLibraryDescriptionList("SDK", "Name", "Version", SDKs, (uint)countof(SDKs));

            LibraryDescription externLibs[] =
            {
                { "D3D12 Memory Allocator", "",                 "https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator" },
                { "D3D12 HDR Sample",       "",                 "https://github.com/microsoft/directx-graphics-samples/tree/master/Samples/Desktop/D3D12HDR" },
                { "Dear ImGui - Docking",   "1.92.5",           "https://github.com/ocornut/imgui/commit/3912b3d9a9c1b3f17431aebafd86d2f40ee6e59c" },
                { "DirectX Shader Compiler","1.8.2505.1",       "https://github.com/microsoft/DirectXShaderCompiler/releases/tag/v1.8.2505.1" },
                { "Dirent",                 "1.23.2",           "https://github.com/tronkko/dirent" },
                { "Font-Awesome",           "6.x",              "https://github.com/FortAwesome/Font-Awesome/tree/6.x" },
                { "fmt",                    "10.1.1",           "https://github.com/fmtlib/fmt/releases/tag/10.1.1" },
                { "FXAA",                   "3.11",             "https://github.com/GameTechDev/CMAA2/blob/master/Projects/CMAA2/FXAA/Fxaa3_11.h" },
                { "hlsl++",                 "3.7",              "https://github.com/redorav/hlslpp/releases/tag/3.7" },
                { "IconFontCppHeaders",     "90da802",          "https://github.com/juliettef/IconFontCppHeaders" },
                { "ImGuiFileDialog",        "0.6.7",            "https://github.com/aiekick/ImGuiFileDialog" },
                { "ImGuizmo",               "1.83",             "https://github.com/CedricGuillemet/ImGuizmo/releases/tag/1.83" },
                { "JoltPhysics",            "5.0.0",            "https://github.com/jrouwe/JoltPhysics/releases/tag/v5.0.0" },
                { "meta_enum_lite",         "2.0 (Custom)",     "https://github.com/vimontgames/meta_enum_lite" },
                { "optick",                 "1.3.1",            "https://github.com/bombomby/optick" },
                { "px_sched",               "f5d3b6e",          "https://github.com/pplux/px" },
                { "Sharpmake",              "0.75.0 (Custom)",  "https://github.com/vimontgames/Sharpmake" },
                { "SoLoud",                 "August 2024",      "https://github.com/jarikomppa/soloud" },
                { "stb_image",              "2.30",             "https://github.com/nothings/stb" },
                { "tinyXML2",               "10.0.0",           "https://github.com/leethomason/tinyxml2" },
                { "UFBX",                   "0.14.3",           "https://github.com/ufbx/ufbx" },
                { "VulkanMemoryAllocator",  "3.0.1",            "https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator" },
                { "WinPixEventRuntime",     "1.0.231030001",    "https://www.nuget.org/packages/WinPixEventRuntime" },
            };

            drawLibraryDescriptionList("Extern libs", "Name", "Version", externLibs, (uint)countof(externLibs));

            LibraryDescription fonts[] =
            {
                { "Font-Awesome",   "6.x",  "https://github.com/FortAwesome/Font-Awesome/tree/6.x" },
                { "Rowdies",        "",     "https://github.com/magictype/rowdy" },
                { "RubikMonoOne",   "",     "https://fonts.google.com/specimen/Rubik+Mono+One" },
                { "UbuntuMono",     "1.0",  "https://fonts.google.com/specimen/Ubuntu+Mono" }
            };

            drawLibraryDescriptionList("Fonts", "Name", "Version", fonts, (uint)countof(fonts));

            ImGui::PopFont();
            End();            
        }
    }
}