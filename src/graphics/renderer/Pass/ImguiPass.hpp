#include "ImguiPass.h"
#include "graphics/renderer/imgui/imguiAdapter.h"
#include "graphics/renderer/Options/DisplayOptions.h"
#include "core/Plugin/Plugin.h"
#include "core/IObjectFactory.h"

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
    void ImguiPass::setup(double _dt)
    {
        writeRenderTarget(0, "Backbuffer");

        ImGuiViewport * viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos, ImGuiCond_Appearing, ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(viewport->WorkSize, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
                         window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
                         window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
                         window_flags |= ImGuiWindowFlags_NoBackground;

        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;

        bool showUI = true;

        //if (m_isDebugWindowVisible)
        //    displayDebugWindow(_dt);

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

                if (ImGui::BeginMenu("Options"))
                {
                    if (ImGui::MenuItem("Display"))
                        m_isDisplayOptionsWindowsVisible = true;

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Tools"))
                {
                    if (ImGui::MenuItem("Debug"))
                        m_isDebugWindowVisible = true;

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Help"))
                {

                    if (ImGui::MenuItem("About VG Framework"))
                        m_isAboutWindowVisible = true;

                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }

            if (m_isAboutWindowVisible)
            {
                if (ImGui::Begin("About", &m_isAboutWindowVisible))
                {
                    float columnWidth[3] = { 256, 128, 512 };

                    ImGui::Text("VG Framework:");
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
                    
                    // name
                    ImGui::Text("D3D12MemoryAllocator");
                    ImGui::Text("Dear ImGui");
                    ImGui::Text("hlslpp");
                    ImGui::Text("magic_enum");
                    ImGui::Text("optick");
                    ImGui::Text("px_sched");
                    ImGui::Text("stb_image");
                    ImGui::Text("VulkanMemoryAllocator");
                    ImGui::Text("WinPixEventRuntime");
                    
                    ImGui::NextColumn();

                    // version
                    ImGui::Text("");
                    ImGui::Text("d5a4d53");
                    ImGui::Text("");
                    ImGui::Text("");
                    ImGui::Text("");
                    ImGui::Text("");
                    ImGui::Text("");
                    ImGui::Text("");
                    ImGui::Text("1.0.200127001");

                    ImGui::NextColumn();

                    // url
                    ImGui::textURL("github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator", "https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator");
                    ImGui::textURL("github.com/ocornut/imgui", "https://github.com/ocornut/imgui");
                    ImGui::textURL("github.com/redorav/hlslpp", "https://github.com/redorav/hlslpp");
                    ImGui::textURL("github.com/Neargye/magic_enum", "https://github.com/Neargye/magic_enum");
                    ImGui::textURL("github.com/bombomby/optick", "https://github.com/bombomby/optick");
                    ImGui::textURL("github.com/pplux/px", "https://github.com/pplux/px");
                    ImGui::textURL("github.com/nothings/stb", "https://github.com/nothings/stb");
                    ImGui::textURL("github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator", "https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator");
                    ImGui::textURL("nuget.org/packages/WinPixEventRuntime", "https://www.nuget.org/packages/WinPixEventRuntime");

                    ImGui::End();
                }
            }

            static const uint smoothDtTime = 500; // 0.5s

            m_accum += _dt;
            m_frame++;

            if (m_accum > (double)smoothDtTime)
            {
                m_dt = (float)(m_accum / (float)m_frame);
                m_fps = (float)1000.0f / m_dt;
                m_accum = 0.0;
                m_frame = 0;
            }          
        }
        ImGui::End();

        if (m_isDebugWindowVisible)
            displayDebugWindow(_dt);

        if (m_isDisplayOptionsWindowsVisible)
            displayOptionsWindow();

        static bool showDemo = false;
        if (showDemo)
            ImGui::ShowDemoWindow(&showDemo);
    }

    //--------------------------------------------------------------------------------------
    void ImguiPass::displayDebugWindow(double _dt)
    {
        if (ImGui::Begin("Debug", &m_isDebugWindowVisible/*, ImGuiWindowFlags_NoTitleBar*/))
        {
            if (ImGui::CollapsingHeader("Target", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Columns(2, "mycolumns2", false);  // 2-ways, no border

                ImGui::Text("Platform:");
                ImGui::Text("Configuration:");
                ImGui::Text("Graphics API:");
                ImGui::NextColumn();
                ImGui::Text(Plugin::getPlatform().c_str());
                ImGui::Text(Plugin::getConfiguration().c_str());
                ImGui::Text(asString(Device::get()->getDeviceParams().api).c_str());
                ImGui::Columns(1);
            }

            if (ImGui::CollapsingHeader("Performance", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Columns(2, "mycolumns2", false);  // 2-ways, no border
                ImGui::Text("FPS: ");
                ImGui::Text("Frame: ");
                ImGui::NextColumn();
                ImGui::Text("%.0f img/sec", m_fps);
                ImGui::Text("%.4f ms", m_dt);

                ImGui::Columns(1);
                ImGui::Text("Press 'F1' to start/stop profiler");

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

            if (ImGui::CollapsingHeader("Shaders", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Text("Press 'F6' to hot reload shaders");
            }
        }
        ImGui::End();
    }

    /*void SetupStyleFromHue()
    {
#if 1
        // FIXME: those should become parameters to the function
        static int hue = 140;
        static float col_main_sat = 180.f / 255.f;
        static float col_main_val = 161.f / 255.f;
        static float col_area_sat = 124.f / 255.f;
        static float col_area_val = 100.f / 255.f;
        static float col_back_sat = 59.f / 255.f;
        static float col_back_val = 40.f / 255.f;

        ImGui::Begin("Hue Style");
        ImGui::SliderInt("master hue", &hue, 0, 255);

        float dummy;
        ImVec4 rgb;
        ImGui::SetColorEditOptions(ImGuiColorEditFlags_HSV);

        ImGui::ColorConvertHSVtoRGB(hue / 255.f, col_main_sat, col_main_val, rgb.x, rgb.y, rgb.z);
        ImGui::ColorEdit3("main", &rgb.x);
        ImGui::ColorConvertRGBtoHSV(rgb.x, rgb.y, rgb.z, dummy, col_main_sat, col_main_val);

        ImGui::ColorConvertHSVtoRGB(hue / 255.f, col_area_sat, col_area_val, rgb.x, rgb.y, rgb.z);
        ImGui::ColorEdit3("area", &rgb.x);
        ImGui::ColorConvertRGBtoHSV(rgb.x, rgb.y, rgb.z, dummy, col_area_sat, col_area_val);

        ImGui::ColorConvertHSVtoRGB(hue / 255.f, col_back_sat, col_back_val, rgb.x, rgb.y, rgb.z);
        ImGui::ColorEdit3("back", &rgb.x);
        ImGui::ColorConvertRGBtoHSV(rgb.x, rgb.y, rgb.z, dummy, col_back_sat, col_back_val);

        ImGui::End();
#endif

        ImGuiStyle& style = ImGui::GetStyle();

        ImVec4 col_text = ImColor::HSV(hue / 255.f, 20.f / 255.f, 235.f / 255.f);
        ImVec4 col_main = ImColor::HSV(hue / 255.f, col_main_sat, col_main_val);
        ImVec4 col_back = ImColor::HSV(hue / 255.f, col_back_sat, col_back_val);
        ImVec4 col_area = ImColor::HSV(hue / 255.f, col_area_sat, col_area_val);

        style.Colors[ImGuiCol_Text] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
        style.Colors[ImGuiCol_TextDisabled] = ImVec4(col_text.x, col_text.y, col_text.z, 0.58f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);
        style.Colors[ImGuiCol_ChildBg] = ImVec4(col_area.x, col_area.y, col_area.z, 0.00f);
        style.Colors[ImGuiCol_PopupBg] = ImVec4(col_area.x, col_area.y, col_area.z, 0.00f);
        style.Colors[ImGuiCol_Border] = ImVec4(col_text.x, col_text.y, col_text.z, 0.30f);
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.68f);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.45f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.45f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(col_main.x, col_main.y, col_main.z, 0.35f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 0.57f);
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.31f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
        style.Colors[ImGuiCol_ComboBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(col_main.x, col_main.y, col_main.z, 0.80f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.24f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
        style.Colors[ImGuiCol_Button] = ImVec4(col_main.x, col_main.y, col_main.z, 0.44f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
        style.Colors[ImGuiCol_Header] = ImVec4(col_main.x, col_main.y, col_main.z, 0.76f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
        style.Colors[ImGuiCol_Column] = ImVec4(col_text.x, col_text.y, col_text.z, 0.32f);
        style.Colors[ImGuiCol_ColumnHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 0.78f);
        style.Colors[ImGuiCol_ColumnActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(col_main.x, col_main.y, col_main.z, 0.20f);
        style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
        style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
        style.Colors[ImGuiCol_CloseButton] = ImVec4(col_text.x, col_text.y, col_text.z, 0.16f);
        style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 0.39f);
        style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
        style.Colors[ImGuiCol_PlotLines] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
        style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
        style.Colors[ImGuiCol_PlotHistogram] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
        style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
        style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.43f);
        style.Colors[ImGuiCol_TooltipBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.92f);
        style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
    }*/

    
    //--------------------------------------------------------------------------------------
    void ImguiPass::displayOptionsWindow()
    {
        if (ImGui::Begin("DisplayOptions", &m_isDisplayOptionsWindowsVisible))
        {
            core::IObject * displayOptions = DisplayOptions::get();

            const auto * factory = Kernel::getObjectFactory();
            const auto * classDesc = factory->getClassDescriptor("DisplayOptions");

            for (uint i = 0; i < classDesc->getPropertyCount(); ++i)
            {
                const auto & prop = classDesc->getProperty(i);
                const auto type = prop->getType();
                const auto name = prop->getName();
                const auto displayName = prop->getDisplayName();
                const auto offset = prop->getOffset();

                switch (type)
                {
                    default:
                        VG_ASSERT_ENUM_NOT_IMPLEMENTED(type);
                        break;

                    case IPropertyDescriptor::Type::Bool:
                    {
                        bool * pBool = (bool*)(uint_ptr(displayOptions) + offset);
                        ImGui::Checkbox(displayName, pBool);
                    };
                    break;

                    case IPropertyDescriptor::Type::Float4:
                    {
                        float * pFloat4 = (float*)(uint_ptr(displayOptions) + offset);
                        ImGui::ColorEdit4(displayName, pFloat4);
                        //ImGui::SliderFloat4(displayName, pFloat4, 0.0f, 1.0f);
                    };
                    break;
                }
            }
        }

        ImGui::End();

        //SetupStyleFromHue();
    }

    //--------------------------------------------------------------------------------------
    void ImguiPass::draw(driver::CommandList * _cmdList) const
    {
        Renderer::get()->getImGuiAdapter()->render(_cmdList);
    }
}