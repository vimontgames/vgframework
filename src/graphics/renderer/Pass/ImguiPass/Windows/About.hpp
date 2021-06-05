namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    void ImguiPass::displayAboutWindow()
    {
        if (ImGui::Begin("About", &m_isAboutWindowVisible))
        {
            float columnWidth[3] = { 256, 128, 512 };

            const auto version = ImguiPass::getEngine()->getVersion();

            ImGui::Text("VG Framework %u.%u", version.major, version.minor);
            ImGui::Text("");

            ImGui::Columns(3, "author", false);

            ImGui::SetColumnWidth(0, columnWidth[0]);
            ImGui::SetColumnWidth(1, columnWidth[1]);
            ImGui::SetColumnWidth(2, columnWidth[2]);

            ImGui::Text("Website");
            ImGui::Text("Twitter");
            ImGui::Text("");

            ImGui::NextColumn();
            ImGui::NextColumn();

            ImGui::textURL("github.com/vimontgames/vgframework", "https://github.com/vimontgames/vgframework");
            ImGui::textURL("Benoit Vimont", "https://twitter.com/benoitvimont");
            ImGui::Text("");

            ImGui::Separator();

            ImGui::Columns(1);
            ImGui::Text("SDKs:");
            ImGui::Text("");

            ImGui::Columns(3, "SDK", false);

            ImGui::SetColumnWidth(0, columnWidth[0]);
            ImGui::SetColumnWidth(1, columnWidth[1]);
            ImGui::SetColumnWidth(2, columnWidth[2]);

            // name                        
            ImGui::Text("FBX SDK");
            ImGui::Text("Vulkan SDK");
            ImGui::Text("Windows 10 SDK");

            ImGui::Text("");
            ImGui::NextColumn();

            // version
            ImGui::Text("2020.0.1");
            ImGui::Text("1.2.148.0");
            ImGui::Text("10.0.17763.0");

            ImGui::Text("");
            ImGui::NextColumn();

            // url
            ImGui::textURL("www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2020-0", "https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2020-0");
            ImGui::textURL("vulkan.lunarg.com/sdk/home", "https://vulkan.lunarg.com/sdk/home");
            ImGui::textURL("developer.microsoft.com/fr-fr/windows/downloads/sdk-archive", "https://developer.microsoft.com/fr-fr/windows/downloads/sdk-archive/");
            ImGui::Text("");

            ImGui::Separator();

            ImGui::Columns(1);
            ImGui::Text("3rd-party libs:");
            ImGui::Text("");

            ImGui::Columns(3, "3rdparties", false);

            ImGui::SetColumnWidth(0, columnWidth[0]);
            ImGui::SetColumnWidth(1, columnWidth[1]);
            ImGui::SetColumnWidth(2, columnWidth[2]);

            struct ThirdPartyInfo
            {
                const char * name;
                const char * version;
                const char * url;
            };

            ThirdPartyInfo thirdParties[] =
            {
                { "D3D12MemoryAllocator",   "",                 "https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator" },
                { "Dear ImGui",             "1ad1429",          "https://github.com/ocornut/imgui/tree/docking" },
                { "Dirent",                 "833b692",          "https://github.com/tronkko/dirent" },
                { "hlslpp",                 "",                 "https://github.com/redorav/hlslpp" },
                { "ImGui-Addons",           "ea0af59",          "https://github.com/gallickgunner/ImGui-Addons" },
                { "magic_enum",             "",                 "https://github.com/Neargye/magic_enum" },
                { "optick",                 "",                 "https://github.com/bombomby/optick" },
                { "px_sched",               "",                 "https://github.com/pplux/px" },
                { "stb_image",              "",                 "https://github.com/nothings/stb" },
                { "VulkanMemoryAllocator",  "",                 "https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator" },
                { "WinPixEventRuntime",     "1.0.200127001",    "https://www.nuget.org/packages/WinPixEventRuntime" },
            };


            for (uint i = 0; i < countof(thirdParties); ++i)
                ImGui::Text(thirdParties[i].name);

            ImGui::NextColumn();

            for (uint i = 0; i < countof(thirdParties); ++i)
                ImGui::Text(thirdParties[i].version);

            ImGui::NextColumn();

            for (uint i = 0; i < countof(thirdParties); ++i)
                ImGui::textURL(thirdParties[i].url + strlen("https://"), thirdParties[i].url);

            ImGui::Text("");

            ImGui::Separator();

            ImGui::Columns(1);
            ImGui::Text("Special Thanks:");
            ImGui::Text("");
            ImGui::Text("To Bob, Ghighi, Marcel and Hamilcar.");

            ImGui::End();
        }
    }
}