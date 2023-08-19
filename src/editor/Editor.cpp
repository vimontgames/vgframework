#include "editor/Precomp.h"
#include "editor.h"
#include "renderer/IRenderer.h"
#include "engine/IEngine.h"

#if !VG_ENABLE_INLINE
#include "Editor.inl"
#endif

#include "editor/ImGui/Extensions/imguiExtensions.h"
#include "editor/ImGui/Window/FPS/ImGuiFPS.h"
#include "editor/ImGui/Window/Plugin/ImGuiPlugin.h"
#include "editor/ImGui/Window/Platform/ImGuiPlatform.h"
#include "editor/ImGui/Window/Inspector/ImGuiInspector.h"
#include "editor/ImGui/Window/DisplayOptions/ImGuiDisplayOptions.h"
#include "editor/ImGui/Window/About/ImGuiAbout.h"
#include "editor/ImGui/Window/Shader/ImGuiShader.h"
#include "editor/ImGui/Window/Scene/ImGuiScene.h"
#include "editor/ImGui/Window/Resource/ImGuiResource.h"
#include "editor/ImGui/Window/View/EditorView/ImGuiEditorView.h"
#include "editor/ImGui/Window/View/GameView/ImGuiGameView.h"
#include "editor/ImGui/Window/Console/ImGuiConsole.h"
#include "editor/ImGui/Toolbar/Main/ImGuiMainToolbar.h"

using namespace vg::core;
using namespace vg::editor;
using namespace ImGui;

#define VG_EDITOR_VERSION_MAJOR 0
#define VG_EDITOR_VERSION_MINOR 12

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
        m_imGuiWindows.push_back(new ImGuiGameView(IconWithText(style::icon::GameView, "Game"), ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry));
        m_imGuiWindows.push_back(new ImGuiEditorView(IconWithText(style::icon::EditorView, "Editor"), ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry));
        m_imGuiWindows.push_back(new ImGuiConsole(IconWithText(style::icon::Console, "Console"), ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry));
        m_imGuiWindows.push_back(new ImGuiAbout("About", ImGuiWindow::None));

        // Add ImGui toolbars
        m_imGuiWindows.push_back(new ImGuiMainToolbar("Main Toolbar", ImGuiWindow::StartVisible));

        // Plugins
        m_imGuiWindows.push_back(new ImGuiPlugin(IconWithText(style::icon::Plugin, "Plugin")));
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

        if (core::IClassDesc * desc = factory->registerPlugin(Editor, "Editor"))
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
        // Get existing Singletons
        Kernel::setSingletons(_singletons);

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

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar;
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        windowFlags |= ImGuiWindowFlags_NoBackground;

        ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

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
                    ImGui::Separator();

                    if (ImGui::MenuItem("Quit"))
                        ImGui::MessageBox(MessageBoxType::YesNo, "Quit", "Are you sure you want to quit program?", []() { Editor::get()->getEngine()->Quit(); return true; });

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Plugins"))
                {
                    ImGui::Separator();

                    if (ImGui::IconMenuItem(style::icon::Plugin, "Plugins"))
                    {
                        auto plugins = getWindow<ImGuiPlugin>();
                        if (nullptr != plugins)
                            plugins->setVisible(true);
                    }
                    ImGui::EndMenu();
                }

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

            ProcessMessageBox();
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