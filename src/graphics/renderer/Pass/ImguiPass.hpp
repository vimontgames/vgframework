#include "ImguiPass.h"
#include "graphics/renderer/imgui/imguiAdapter.h"
#include "core/Plugin/Plugin.h"

namespace ImGui
{
    //--------------------------------------------------------------------------------------
    void underLine(ImColor _color)
    {
        u32 color = _color;
        ImVec2 min = ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();
        min.y = max.y;
        ImGui::GetWindowDrawList()->AddLine(min, max, color, 1.0f);
    }

    //--------------------------------------------------------------------------------------
    // Add web link. Use '_text' as URL if no URL provided.
    //--------------------------------------------------------------------------------------
    void textURL(const char * _text, const char * _url = nullptr)
    {
        if (nullptr == _url)
            _url = _text;

        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
        ImGui::Text(_text);
        ImGui::PopStyleColor();

        if (ImGui::IsItemHovered())
        {
            underLine(ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);

            if (ImGui::IsMouseClicked(0))
            {
                char tmp[1024];
                sprintf(tmp, "start %s", _url);
                system(tmp);
            }
            ImGui::SetTooltip(_url);
        }
        else
        {
            underLine(ImGui::GetStyle().Colors[ImGuiCol_Button]);
        }
    }
}

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    void ImguiPass::setup()
    {
        writeRenderTarget(0, "Backbuffer");

        // Dear ImGui

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        ImGuiViewport * viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetWorkPos());
        ImGui::SetNextWindowSize(viewport->GetWorkSize());
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        window_flags |= ImGuiWindowFlags_NoBackground;

        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;

        bool showUI = true;

        ImGui::Begin("DockSpace", &showUI, window_flags);
        {
            ImGui::PopStyleVar(3);

            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    ImGui::MenuItem("New");

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Windows"))
                {
                    if (ImGui::MenuItem("Debug"))
                        m_isDebugWindowVisible = true;

                    if (ImGui::MenuItem("About"))
                        m_isAboutWindowVisible = true;

                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }

            if (m_isAboutWindowVisible)
            {
                if (ImGui::Begin("About", &m_isAboutWindowVisible))
                {
                    ImGui::Text("VimontGames Framework:");
                    ImGui::Text("");
 
                    ImGui::Columns(2, "mycolumns2", false);  
                    
                    ImGui::Text("Website");
                    ImGui::Text("Author");
                    ImGui::Text("");

                    ImGui::NextColumn();

                    ImGui::textURL("github.com/vimontgames/vgframework", "https://github.com/vimontgames/vgframework");
                    ImGui::textURL("Benoit Vimont", "https://twitter.com/benoitvimont");
                    ImGui::Text("");

                    ImGui::Separator();

                    ImGui::Columns(1);
                    ImGui::Text("Software development kits:");
                    ImGui::Text("");

                    ImGui::Columns(2, "mycolumns2", false);
                    ImGui::SetColumnWidth(0, 200);

                    ImGui::Text("Windows 10 SDK");
                    ImGui::Text("Vulkan SDK");
                    ImGui::Text("");

                    ImGui::NextColumn();

                    ImGui::textURL("developer.microsoft.com/fr-fr/windows/downloads/sdk-archive", "https://developer.microsoft.com/fr-fr/windows/downloads/sdk-archive/");
                    ImGui::textURL("vulkan.lunarg.com/sdk/home", "https://vulkan.lunarg.com/sdk/home");
                    ImGui::Text("");

                    ImGui::Separator();

                    ImGui::Columns(1);
                    ImGui::Text("Third-party libraries:");
                    ImGui::Text("");

                    ImGui::Columns(2, "mycolumns2", false);
                    
                    ImGui::Text("Dear ImGui");
                    ImGui::Text("optick");
                    ImGui::Text("stb_image");
                    ImGui::Text("hlslpp");
                    ImGui::Text("magic_enum");
                    ImGui::Text("D3D12MemoryAllocator");
                    ImGui::Text("VulkanMemoryAllocator");
                    
                    ImGui::Text("");

                    ImGui::NextColumn();

                    ImGui::textURL("github.com/ocornut/imgui", "https://github.com/ocornut/imgui");
                    ImGui::textURL("github.com/bombomby/optick", "https://github.com/bombomby/optick");
                    ImGui::textURL("github.com/nothings/stb", "https://github.com/nothings/stb");
                    ImGui::textURL("github.com/redorav/hlslpp", "https://github.com/redorav/hlslpp");
                    ImGui::textURL("github.com/Neargye/magic_enum", "https://github.com/Neargye/magic_enum");
                    ImGui::textURL("github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator", "https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator");
                    ImGui::textURL("github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator", "https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator");

                    //ImGui::Text("");

                    ImGui::End();
                }
            }

            if (m_isDebugWindowVisible)
                displayDebugWindow();            
        }
        ImGui::End();

        //bool showDemo = true;
        //ImGui::ShowDemoWindow(&showDemo);
    }

    //--------------------------------------------------------------------------------------
    void ImguiPass::displayDebugWindow()
    {
        if (ImGui::Begin("Debug", &m_isDebugWindowVisible/*, ImGuiWindowFlags_NoTitleBar*/))
        {
            if (ImGui::CollapsingHeader("Target", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Columns(2, "mycolumns2", false);  // 2-ways, no border

                ImGui::Text("Platform:");
                ImGui::Text("Configuration");
                ImGui::Text("Graphics API");
                ImGui::NextColumn();
                ImGui::Text(Plugin::getPlatform().c_str());
                ImGui::Text(Plugin::getConfiguration().c_str());
                ImGui::Text(asString(Device::get()->getDeviceParams().api).c_str());
                ImGui::Columns(1);
            }

            if (ImGui::CollapsingHeader("Profiler", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Text("Press 'F1' to start/stop capture");

                if (VG_PROFILE_CAPTURE_IN_PROGRESS())
                {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Capture in progress ... %u", m_captureFrameCounter);
                    m_captureFrameCounter++;
                }
                else if (0 != m_captureFrameCounter)
                {
                    m_captureFrameCounter = 0;
                }
            }

            ImGui::End();
        }
    }

    //--------------------------------------------------------------------------------------
    void ImguiPass::draw(driver::CommandList * _cmdList) const
    {
        VG_PROFILE_GPU("Dear ImGui");
        Renderer::get()->getImGuiAdapter()->render(_cmdList);
    }
}