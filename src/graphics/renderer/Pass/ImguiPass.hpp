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
        auto * renderer = Renderer::get();
        const auto & backbuffer = renderer->getBackbuffer()->getTexDesc();

        FrameGraph::TextureDesc desc;
        desc.width = backbuffer.width;
        desc.height = backbuffer.height;
        desc.format = backbuffer.format;

        desc.initState = FrameGraph::Resource::InitState::Preserve;
        writeRenderTarget(0, "Backbuffer", desc);

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
                    ImGui::Text("VGFramework");

                    ImGui::Separator();

                    ImGui::Columns(2, "mycolumns2", false);  // 2-ways, no border
                    ImGui::Text("Author");
                    ImGui::Text("Website");

                    ImGui::NextColumn();

                    ImGui::textURL("Benoit Vimont", "https://twitter.com/benoitvimont");
                    ImGui::textURL("github.com/vimontgames/vgframework", "https://github.com/vimontgames/vgframework");

                    ImGui::Separator();

                    ImGui::Columns(1);
                    ImGui::Text("Third-party libraries used:");
                    ImGui::Columns(2, "mycolumns2", false);  // 2-ways, no border

                    ImGui::Text("User interface");
                    ImGui::Text("C++ enum reflection");
                    ImGui::Text("C++ hlsl-like math library");
                    ImGui::Text("Memory (GPU)");
                    ImGui::Text("Memory (GPU)");
                    ImGui::Text("Profiler");

                    ImGui::NextColumn();

                    ImGui::textURL("Dear ImGui", "https://github.com/ocornut/imgui");
                    ImGui::textURL("magic_enum", "https://github.com/Neargye/magic_enum");
                    ImGui::textURL("hlslpp", "https://github.com/redorav/hlslpp");
                    ImGui::textURL("D3D12MemoryAllocator", "https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator");
                    ImGui::textURL("VulkanMemoryAllocator", "https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator");
                    ImGui::textURL("optick", "https://github.com/bombomby/optick");
                    
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
        Renderer::get()->m_imgui->render(_cmdList);
    }
}