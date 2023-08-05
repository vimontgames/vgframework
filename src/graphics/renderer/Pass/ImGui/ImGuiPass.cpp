#include "graphics/renderer/Precomp.h"
#include "ImguiPass.h"
#include "gfx/Device/Device.h"
#include "graphics/renderer/Renderer.h"
#include "graphics/renderer/Geometry/Batch/Batch.h"
#include "graphics/renderer/imgui/imguiAdapter.h"
#include "graphics/renderer/Options/DisplayOptions.h"
#include "engine/IEngine.h"
#include "core/Plugin/Plugin.h"
#include "core/IFactory.h"
#include "core/IResource.h"
#include "core/IUniverse.h"
#include "core/IScene.h"
#include "core/IGameObject.h"
#include "corE/IComponent.h"
#include "core/File/File.h"
#include "core/Math/Math.h"

using namespace vg::core;
using namespace vg::gfx;
using namespace vg::graphics::renderer;

#include "imgui/imgui.h"
#include "graphics/renderer/ImGui/Editors/ImguiEditor.h"

using namespace ImGui;

#include "graphics/renderer/ImGui/Editors/Scene/ImguiScene.h"
#include "graphics/renderer/ImGui/Editors/Resource/ImguiResource.h"
#include "graphics/renderer/ImGui/Editors/Platform/ImguiPlatform.h"
#include "graphics/renderer/ImGui/Editors/DisplayOptions/ImguiDisplayOptions.h"
#include "graphics/renderer/ImGui/Editors/Shader/ImguiShader.h"
#include "graphics/renderer/ImGui/Editors/FPS/ImguiFPS.h"
#include "graphics/renderer/ImGui/Editors/Inspector/ImguiInspector.h"
#include "graphics/renderer/ImGui/Editors/About/ImguiAbout.h"
#include "graphics/renderer/ImGui/Editors/View/ImGuiView.h"
#include "graphics/renderer/ImGui/Editors/View/EditorView/ImGuiEditorView.h"
#include "graphics/renderer/ImGui/Editors/View/GAmeView/ImGuiGameView.h"

#include "graphics/renderer/ImGui/Toolbars/Main/ImGuiMainToolbar.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    // In case several windows are docked together, then the last declared will be the default selected
    //--------------------------------------------------------------------------------------
    ImguiPass::ImguiPass() :
        gfx::UserPass("ImGuiPass")
    {
        // Add ImGui editors
        m_imGuiEditors.push_back(new ImguiPlatform(IconWithText(Editor::Icon::Platform, "Platform"), ImguiEditor::StartVisible | ImguiEditor::AddMenuEntry));
        m_imGuiEditors.push_back(new ImguiShader(IconWithText(Editor::Icon::Shaders, "Shaders"), ImguiEditor::StartVisible | ImguiEditor::AddMenuEntry));
        m_imGuiEditors.push_back(new ImguiFPS(IconWithText(Editor::Icon::FPS, "FPS"), ImguiEditor::StartVisible | ImguiEditor::AddMenuEntry));
        m_imGuiEditors.push_back(new ImguiResource(IconWithText(Editor::Icon::Resource, "Resources"), ImguiEditor::StartVisible | ImguiEditor::AddMenuEntry));
        m_imGuiEditors.push_back(new ImguiScene(IconWithText(Editor::Icon::Scene,"Scenes"), ImguiEditor::StartVisible | ImguiEditor::AddMenuEntry));
        m_imGuiEditors.push_back(new ImguiInspector(IconWithText(Editor::Icon::Inspector, "Inspector"), ImguiEditor::StartVisible | ImguiEditor::AddMenuEntry));
        m_imGuiEditors.push_back(new ImguiDisplayOptions(IconWithText(Editor::Icon::Display, "Display"), ImguiEditor::StartVisible));
        m_imGuiEditors.push_back(new ImguiAbout("About", ImguiEditor::None));
        m_imGuiEditors.push_back(new ImGuiEditorView(IconWithText(Editor::Icon::EditorView, "Editor View"), ImguiEditor::StartVisible | ImguiEditor::AddMenuEntry));
        m_imGuiEditors.push_back(new ImGuiGameView(IconWithText(Editor::Icon::GameView, "Game View"), ImguiEditor::StartVisible | ImguiEditor::AddMenuEntry));

        // Add ImGui toolbars
        m_imGuiToolbars.push_back(new ImGuiMainToolbar("Main Toolbar", ImguiEditor::StartVisible | ImguiEditor::AddMenuEntry));
    }

    //--------------------------------------------------------------------------------------
    ImguiPass::~ImguiPass()
    {
        for (uint i = 0; i < m_imGuiEditors.count(); ++i)
            VG_SAFE_DELETE(m_imGuiEditors[i]);
        m_imGuiEditors.clear();

        for (uint i = 0; i < m_imGuiToolbars.count(); ++i)
            VG_SAFE_DELETE(m_imGuiToolbars[i]);
        m_imGuiToolbars.clear();
    }
    
    //--------------------------------------------------------------------------------------
    void ImguiPass::setup(const gfx::RenderContext & _renderContext, double _dt)
    {
        writeRenderTarget(0, "Backbuffer");

        ImGuiViewport * viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos, ImGuiCond_Appearing, ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(viewport->WorkSize, ImGuiCond_None);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.5f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        window_flags |= ImGuiWindowFlags_NoBackground;

        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode/*  | ImGuiDockNodeFlags_AutoHideTabBar*/;

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

                    ImGui::EndMenu();
                }

                //if (ImGui::BeginMenu("Plugins"))
                //{
                //    if (ImGui::MenuItem("Engine"))
                //        m_isEngineWindowVisible = true;
                //
                //    if (ImGui::MenuItem("Renderer"))
                //        m_isRendererWindowVisible = true;
                //
                //    ImGui::EndMenu();
                //}

                if (ImGui::BeginMenu("Window"))
                {
                    for (uint i = 0; i < m_imGuiEditors.count(); ++i)
                    {
                        if (asBool(m_imGuiEditors[i]->getFlags() & ImguiEditor::Flags::AddMenuEntry))
                        {
                            if (ImGui::MenuItem(m_imGuiEditors[i]->getName().c_str()))
                                m_imGuiEditors[i]->setVisible(true);
                        }
                    }

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Options"))
                {
                    if (ImGui::IconMenuItem(Editor::Icon::Display, "Display"))
                    {
                        ImguiDisplayOptions * displayOptions = getEditorWindow<ImguiDisplayOptions>();
                        if (nullptr != displayOptions)
                            displayOptions->setVisible(true);
                    }

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Help"))
                {
                    if (ImGui::IconMenuItem(Editor::Icon::About, "About"))
                    {
                        ImguiAbout * about = getEditorWindow<ImguiAbout>();
                        if (nullptr != about)
                            about->setVisible(true);
                    }

                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }

            ImGuiAxis axis = ImGuiAxis_X;

        }
        ImGui::End();

        for (uint i = 0; i < m_imGuiEditors.size(); ++i)
        {
            if (m_imGuiEditors[i]->isVisible())
            {
                m_imGuiEditors[i]->update(_dt);
                m_imGuiEditors[i]->display();
            }
        }

        for (uint i = 0; i < m_imGuiToolbars.size(); ++i)
        {
            if (m_imGuiToolbars[i]->isVisible())
            {
                m_imGuiToolbars[i]->update(_dt);
                m_imGuiToolbars[i]->display();
            }
        }

        if (m_isEngineWindowVisible)
            displayEngineWindow();

        if (m_isRendererWindowVisible)
            displayRendererWindow();

        static bool demo = false;
        if (demo)
            ImGui::ShowDemoWindow(&demo);
    }

    //--------------------------------------------------------------------------------------
    template <class T> T * ImguiPass::getEditorWindow()
    {
        for (uint i = 0; i < m_imGuiEditors.count(); ++i)
        {
            if (dynamic_cast<T*>(m_imGuiEditors[i]) != nullptr)
                return (T*)(m_imGuiEditors[i]);
        }
        return nullptr;
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
    void ImguiPass::draw(const RenderContext & _renderContext, gfx::CommandList * _cmdList) const
    {
        Renderer::get()->getImGuiAdapter()->render(_cmdList);
    }
}