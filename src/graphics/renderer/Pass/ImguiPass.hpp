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
        ImGui::SetNextWindowSize(viewport->WorkSize, ImGuiCond_None);
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
    
    //--------------------------------------------------------------------------------------
    void ImguiPass::displayOptionsWindow()
    {
        core::IObject * displayOptions = DisplayOptions::get();
        if (displayOptions)
            displayObject(displayOptions, &m_isDisplayOptionsWindowsVisible);
    }

    //--------------------------------------------------------------------------------------
    void ImguiPass::displayObject(core::IObject * _object, bool * _open)
    {
        const char * className = _object->getClassName();

        if (ImGui::Begin(className, _open))
        {
            const auto * factory = Kernel::getObjectFactory();
            const auto * classDesc = factory->getClassDescriptor(className);

            for (uint i = 0; i < classDesc->getPropertyCount(); ++i)
            {
                const auto & prop = classDesc->getProperty(i);
                const auto type = prop->getType();
                const auto name = prop->getName();
                const auto displayName = prop->getDisplayName();
                const auto offset = prop->getOffset();
                const auto flags = prop->getFlags();

                switch (type)
                {
                    default:
                        VG_ASSERT_ENUM_NOT_IMPLEMENTED(type);
                        break;

                    case IPropertyDescriptor::Type::Bool:
                    {
                        bool * pBool = (bool*)(uint_ptr(_object) + offset);
                        ImGui::Checkbox(displayName, pBool);
                    };
                    break;

                    case IPropertyDescriptor::Type::Float4:
                    {
                        float * pFloat4 = (float*)(uint_ptr(_object) + offset);

                        if (asBool(IPropertyDescriptor::Flags::Color & flags))
                            ImGui::ColorEdit4(displayName, pFloat4);
                        else
                            ImGui::SliderFloat4(displayName, pFloat4, 0.0f, 1.0f);
                    };
                    break;

                    case IPropertyDescriptor::Type::String:
                    {
                        string * pString = (string*)(uint_ptr(_object) + offset);

                        ImGuiInputTextFlags imguiflags = 0;

                        if (asBool(IPropertyDescriptor::Flags::ReadOnly & flags))
                            imguiflags = ImGuiInputTextFlags_ReadOnly;

                        char buffer[1024];
                        sprintf_s(buffer, pString->c_str());
                        ImGui::InputText(displayName, buffer, countof(buffer), imguiflags);
                        *pString = buffer;
                    }
                    break;
                }
            }
        }

        ImGui::End();
    }

    //--------------------------------------------------------------------------------------
    void ImguiPass::draw(driver::CommandList * _cmdList) const
    {
        Renderer::get()->getImGuiAdapter()->render(_cmdList);
    }
}