#include "editor/Precomp.h"
#include "editor.h"
#include "renderer/IRenderer.h"

#if !VG_ENABLE_INLINE
#include "Editor.inl"
#endif

#include "imgui/imgui.cpp"
#include "imgui/imgui_demo.cpp"
#include "imgui/imgui_draw.cpp"
#include "imgui/imgui_tables.cpp"
#include "imgui/imgui_widgets.cpp"

#include "editor/ImGui/Extensions/imguiExtensions.h"
#include "editor/ImGui/Window/FPS/ImGuiFPS.h"
#include "editor/ImGui/Window/Platform/ImGuiPlatform.h"
#include "editor/ImGui/Window/Inspector/ImGuiInspector.h"
#include "editor/ImGui/Window/DisplayOptions/ImGuiDisplayOptions.h"
#include "editor/ImGui/Window/About/ImguiAbout.h"
#include "editor/ImGui/Window/Shader/ImGuiShader.h"
#include "editor/ImGui/Window/Scene/ImGuiScene.h"
#include "editor/ImGui/Window/Resource/ImGuiResource.h"
#include "editor/ImGui/Window/View/EditorView//ImGuiEditorView.h"
#include "editor/ImGui/Window/View/GameView//ImGuiGameView.h"
#include "editor/ImGui/Toolbar/Main/ImGuiMainToolbar.h"

using namespace vg::core;
using namespace vg::editor;
using namespace ImGui;

#define VG_EDITOR_VERSION_MAJOR 0
#define VG_EDITOR_VERSION_MINOR 1

//--------------------------------------------------------------------------------------
IEditor * CreateNew()
{
	return new Editor("Editor", nullptr);
}

//--------------------------------------------------------------------------------------
namespace vg::editor
{
	//--------------------------------------------------------------------------------------
	IPlugin::Version Editor::GetVersion() const
	{
		return { VG_EDITOR_VERSION_MAJOR, VG_EDITOR_VERSION_MINOR };
	}

    #ifdef _WIN32
    //--------------------------------------------------------------------------------------
    LRESULT CALLBACK Editor::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
            case WM_SIZE:
                break;

            case WM_KEYDOWN:
                switch (wParam)
                {
                    case VK_F1:
                        break;

                    case VK_F6:
                        break;
                }
                break;
        }

        return 0;
    }
    #endif

	//--------------------------------------------------------------------------------------
	Editor::Editor(const core::string & _name, core::IObject * _parent) :
        IEditor(_name, _parent)
	{
        // Add ImGui editors
        m_imGuiWindows.push_back(new ImGuiPlatform(IconWithText(style::icon::Platform, "Platform"), ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry));
        m_imGuiWindows.push_back(new ImGuiShader(IconWithText(style::icon::Shaders, "Shaders"), ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry));
        m_imGuiWindows.push_back(new ImGuiFPS(IconWithText(style::icon::FPS, "FPS"), ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry));
        m_imGuiWindows.push_back(new ImGuiResource(IconWithText(style::icon::Resource, "Resources"), ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry));
        m_imGuiWindows.push_back(new ImGuiScene(IconWithText(style::icon::Scene,"Scenes"), ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry));
        m_imGuiWindows.push_back(new ImGuiInspector(IconWithText(style::icon::Inspector, "Inspector"), ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry));
        m_imGuiWindows.push_back(new ImGuiDisplayOptions(IconWithText(style::icon::Display, "Display"), ImGuiWindow::StartVisible));
        m_imGuiWindows.push_back(new ImGuiEditorView(IconWithText(style::icon::EditorView, "Editor View"), ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry));
        m_imGuiWindows.push_back(new ImGuiGameView(IconWithText(style::icon::GameView, "Game View"), ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry));
        m_imGuiWindows.push_back(new ImGuiAbout("About", ImGuiWindow::None));

        // Add ImGui toolbars
        m_imGuiWindows.push_back(new ImGuiMainToolbar("Main Toolbar", ImGuiWindow::StartVisible));
	}

	//--------------------------------------------------------------------------------------
    Editor::~Editor()
	{
        for (uint i = 0; i < m_imGuiWindows.count(); ++i)
            VG_SAFE_DELETE(m_imGuiWindows[i]);
        m_imGuiWindows.clear();
	}

    //--------------------------------------------------------------------------------------
    bool Editor::RegisterClasses()
    {
        core::IFactory * factory = Kernel::getFactory();

        // Register classes to auto-register the "Engine" module
        AutoRegisterClassInfo::registerClasses(*factory);

        if (core::IClassDesc * desc = factory->registerClassSingletonHelper(Editor, "Editor", IClassDesc::Flags::None))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Editor::UnregisterClasses()
    {
        IFactory * factory = Kernel::getFactory();
        return AutoRegisterClassInfo::unregisterClasses(*factory);
    }

	//--------------------------------------------------------------------------------------
	void Editor::Init(const core::Singletons & _singletons)
	{
        // Singletons used by the editor DLL
        Kernel::setScheduler(_singletons.scheduler);
        Kernel::setInput(_singletons.input);
        Kernel::setFactory(_singletons.factory);
        Kernel::setProfiler(_singletons.profiler);

        RegisterClasses();
	}

	//--------------------------------------------------------------------------------------
	void Editor::Deinit()
	{
        UnregisterClasses();
	}

    //--------------------------------------------------------------------------------------
    template <class T> T * Editor::getWindow() const
    {
        for (uint i = 0; i < m_imGuiWindows.count(); ++i)
        {
            if (dynamic_cast<T *>(m_imGuiWindows[i]) != nullptr)
                return (T *)(m_imGuiWindows[i]);
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    vg::core::IFactory * Editor::getFactory()
    {
        return Kernel::getFactory();
    }

    //--------------------------------------------------------------------------------------
    vg::engine::IEngine * Editor::getEngine()
    {
        return (vg::engine::IEngine *)getFactory()->getSingleton("Engine");
    }

    //--------------------------------------------------------------------------------------
    vg::renderer::IRenderer * Editor::getRenderer()
    {
        return (vg::renderer::IRenderer *)getFactory()->getSingleton("Renderer");
    }

	//--------------------------------------------------------------------------------------
	void Editor::DrawGUI(const GUIContext & _context)
	{
        VG_PROFILE_FRAME("Editor");
        VG_PROFILE_CPU("Editor");

        auto * imGuiContext = (ImGuiContext*)_context.ptr;
        ImGui::SetCurrentContext(imGuiContext);

        ImGuiViewport * viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos, ImGuiCond_Appearing, ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(viewport->WorkSize, ImGuiCond_None);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.5f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGuiWindowFlags windowFlags  = ImGuiWindowFlags_MenuBar;
                         windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
                         windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
                         windowFlags |= ImGuiWindowFlags_NoBackground;

        ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;

        bool showUI = true;

        ImGui::Begin("EditorDockSpace", &showUI, windowFlags);
        {
            ImGui::PopStyleVar(3);

            ImGuiID dockspaceId = ImGui::GetID("EditorDockSpace");
            ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);

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
                    for (uint i = 0; i < m_imGuiWindows.count(); ++i)
                    {
                        if (asBool(m_imGuiWindows[i]->getFlags() & ImGuiWindow::Flags::AddMenuEntry))
                        {
                            if (ImGui::MenuItem(m_imGuiWindows[i]->getName().c_str()))
                                m_imGuiWindows[i]->setVisible(true);
                        }
                    }

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Options"))
                {
                    if (ImGui::IconMenuItem(style::icon::Display, "Display"))
                    {
                        ImGuiDisplayOptions * displayOptions = getWindow<ImGuiDisplayOptions>();
                        if (nullptr != displayOptions)
                            displayOptions->setVisible(true);
                    }

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Help"))
                {
                    if (ImGui::IconMenuItem(style::icon::About, "About"))
                    {
                        ImGuiAbout * about = getWindow<ImGuiAbout>();
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

        for (uint i = 0; i < m_imGuiWindows.size(); ++i)
        {
            if (m_imGuiWindows[i]->isVisible())
                m_imGuiWindows[i]->DrawGUI();
        }

        static bool demo = false;
        if (demo)
            ImGui::ShowDemoWindow(&demo);
	}
}