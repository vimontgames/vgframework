#include "graphics/renderer/Precomp.h"
#include "ImguiPass.h"
#include "graphics/driver/Device/Device.h"
#include "graphics/renderer/Renderer.h"
#include "graphics/renderer/Geometry/Batch.h"
#include "graphics/renderer/imgui/imguiAdapter.h"
#include "graphics/renderer/Options/DisplayOptions.h"
#include "engine/IEngine.h"
#include "core/Plugin/Plugin.h"
#include "core/IFactory.h"
#include "core/IResource.h"
#include "core/IUniverse.h"
#include "core/IScene.h"
#include "core/ISector.h"
#include "core/IEntity.h"
#include "corE/IComponent.h"
#include "core/File/File.h"
#include "core/Math/Math.h"

using namespace vg::core;
using namespace vg::graphics::driver;
using namespace vg::graphics::renderer;

#include "imgui/imgui.h"
#include "Windows/ImguiEditor.h"

using namespace ImGui;

#include "Windows/ImguiScene.hpp"
#include "Windows/ImguiResource.hpp"
#include "Windows/ImguiDisplayOptions.hpp"
#include "Windows/ImguiInspector.hpp"
#include "Windows/ImguiAbout.hpp"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    ImguiPass::ImguiPass()
    {
        for (uint i = 0; i < core::enumCount<UIWindow>(); ++i)
            m_isWindowVisible[i] = true;

        // Add editor windows
        m_editorWindows.push_back(new ImguiScene("Scenes", ImguiEditor::Flags::StartVisible | ImguiEditor::AddMenuEntry));
        m_editorWindows.push_back(new ImguiResource("Resources", ImguiEditor::Flags::StartVisible | ImguiEditor::AddMenuEntry));
        m_editorWindows.push_back(new ImguiInspector("Inspector", ImguiEditor::Flags::StartVisible | ImguiEditor::AddMenuEntry));
        m_editorWindows.push_back(new ImguiDisplayOptions("Display Options", ImguiEditor::Flags::StartVisible));
        m_editorWindows.push_back(new ImguiAbout("About", ImguiEditor::Flags::None));
    }

    //--------------------------------------------------------------------------------------
    ImguiPass::~ImguiPass()
    {
        for (uint i = 0; i < m_editorWindows.count(); ++i)
            VG_SAFE_DELETE(m_editorWindows[i]);
        m_editorWindows.clear();
    }
    
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
                if (ImGui::BeginMenu("Plugins"))
                {
                    if (ImGui::MenuItem("Engine"))
                        m_isEngineWindowVisible = true;

                    if (ImGui::MenuItem("Renderer"))
                        m_isRendererWindowVisible = true;

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Windows"))
                {
                    for (uint i = 0; i < enumCount<UIWindow>(); ++i)
                    {
                        if (ImGui::MenuItem(asString(UIWindow(i)).c_str()))
                            m_isWindowVisible[i] = true;
                    }

                    for (uint i = 0; i < m_editorWindows.count(); ++i)
                    {
                        if (asBool(m_editorWindows[i]->getFlags() & ImguiEditor::Flags::AddMenuEntry))
                        {
                            if (ImGui::MenuItem(m_editorWindows[i]->getName().c_str()))
                                m_editorWindows[i]->setVisible(true);
                        }
                    }

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Options"))
                {
                    if (ImGui::MenuItem("Display Options"))
                    {
                        ImguiDisplayOptions * displayOptions = getEditorWindow<ImguiDisplayOptions>();
                        if (nullptr != displayOptions)
                            displayOptions->setVisible(true);
                    }

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Help"))
                {
                    if (ImGui::MenuItem("About "))
                    {
                        ImguiAbout * about = getEditorWindow<ImguiAbout>();
                        if (nullptr != about)
                            about->setVisible(true);
                    }

                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }

            static const uint smoothDtTime = 1000; // 1.0s

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

        for (uint i = 0; i < m_editorWindows.count(); ++i)
        {
            if (m_editorWindows[i]->isVisible())
                m_editorWindows[i]->display();
        }

        if (m_isEngineWindowVisible)
            displayEngineWindow();

        if (m_isRendererWindowVisible)
            displayRendererWindow();

        if (m_isWindowVisible[asInteger(UIWindow::Platform)])
            displayPlatformWindow();

        if (m_isWindowVisible[asInteger(UIWindow::Shaders)])
            displayShadersWindow();

        if (m_isWindowVisible[asInteger(UIWindow::FPS)])
            displayFpsWindow();

        static bool showDemo = false;
        if (showDemo)
            ImGui::ShowDemoWindow(&showDemo);
    }

    //--------------------------------------------------------------------------------------
    template <class T> T * ImguiPass::getEditorWindow()
    {
        for (uint i = 0; i < m_editorWindows.count(); ++i)
        {
            if (dynamic_cast<T*>(m_editorWindows[i]) != nullptr)
                return (T*)(m_editorWindows[i]);
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    void ImguiPass::displayPlatformWindow()
    {
        if (ImGui::Begin("Platform", &m_isWindowVisible[asInteger(UIWindow::Platform)]))
        {
            ImGui::Columns(2, "mycolumns2", false);  // 2-ways, no border

            ImGui::Text("Platform:");
            ImGui::Text("Configuration:");
            ImGui::Text("Graphics API:");
            ImGui::NextColumn();
            ImGui::Text(Plugin::getPlatform().c_str());
            ImGui::Text(Plugin::getConfiguration().c_str());
            ImGui::Text(asString(Device::get()->getDeviceParams().api).c_str());
        }
        ImGui::End();
    }

    //--------------------------------------------------------------------------------------
    void ImguiPass::displayFpsWindow()
    {
        if (ImGui::Begin("FPS", &m_isWindowVisible[asInteger(UIWindow::FPS)]))
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
        ImGui::End();
    }

    //--------------------------------------------------------------------------------------
    void ImguiPass::displayShadersWindow()
    {
        if (ImGui::Begin("Shaders", &m_isWindowVisible[asInteger(UIWindow::Shaders)]))
        {
            ImGui::Text("Press 'F6' to hot reload shaders");
        }
        ImGui::End();
    }

    //--------------------------------------------------------------------------------------
    const vg::engine::IEngine * ImguiPass::getEngine()
    {
        const auto * factory = Kernel::getFactory();
        return (const vg::engine::IEngine *) factory->getSingleton("Engine");
    }

    //--------------------------------------------------------------------------------------
    void ImguiPass::displayEngineWindow()
    {
        if (ImGui::Begin("Engine", &m_isEngineWindowVisible))
        {
            const auto * factory = Kernel::getFactory();
            core::IObject * engine = (core::IObject *)getEngine();
            if (engine)
            {
                const char * className = engine->getClassName();
                const auto * classDesc = factory->getClassDescriptor(className);
                const char * classDisplayName = classDesc->getClassDisplayName();

                ImguiEditor::displayObject(engine);
            }
        }
        ImGui::End();
    }

    //--------------------------------------------------------------------------------------
    void ImguiPass::displayRendererWindow()
    {
        if (ImGui::Begin("Renderer", &m_isRendererWindowVisible))
        {
            const auto * factory = Kernel::getFactory();
            IObject * renderer = factory->getSingleton("Renderer");
            if (renderer)
                ImguiEditor::displayObject(renderer);
        }
        ImGui::End();
    }
    
    //--------------------------------------------------------------------------------------
    void ImguiPass::draw(driver::CommandList * _cmdList) const
    {
        Renderer::get()->getImGuiAdapter()->render(_cmdList);
    }
}