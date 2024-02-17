#include "editor/Precomp.h"
#include "editor.h"
#include "core/IInput.h"
#include "core/IResource.h"
#include "core/File/File.h"
#include "renderer/IRenderer.h"
#include "renderer/IImGuiAdapter.h"
#include "engine/IEngine.h"
#include "engine/IWorldResource.h"
#include "editor/ImGui/Extensions/ImGuizmo/ImGuizmoAdapter.h"
#include "editor/Options/EditorOptions.h"
#include "ImGui-Addons/FileBrowser/ImGuiFileBrowser.h"

#if !VG_ENABLE_INLINE
#include "Editor.inl"
#endif

#include "editor/ImGui/Extensions/imguiExtensions.h"
#include "editor/ImGui/Window/FPS/ImGuiFPS.h"
#include "editor/ImGui/Window/Plugin/ImGuiPlugin.h"
#include "editor/ImGui/Window/Platform/ImGuiPlatform.h"
#include "editor/ImGui/Window/Inspector/ImGuiInspector.h"
#include "editor/ImGui/Window/Options/Editor/ImGuiEditorOptions.h"
#include "editor/ImGui/Window/Options/Engine/ImGuiEngineOptions.h"
#include "editor/ImGui/Window/Options/Renderer/ImGuiRendererOptions.h"
#include "editor/ImGui/Window/Options/Physics/ImGuiPhysicsOptions.h"
#include "editor/ImGui/Window/About/ImGuiAbout.h"
#include "editor/ImGui/Window/Shader/ImGuiShader.h"
#include "editor/ImGui/Window/SceneList/World/ImGuiWorld.h"
#include "editor/ImGui/Window/SceneList/Prefab/ImGuiPrefab.h"
#include "editor/ImGui/Window/Resource/ImGuiResource.h"
#include "editor/ImGui/Window/View/EditorView/ImGuiEditorView.h"
#include "editor/ImGui/Window/View/GameView/ImGuiGameView.h"
#include "editor/ImGui/Window/Console/ImGuiConsole.h"
#include "editor/ImGui/Window/Input/ImGuiInput.h"
#include "editor/ImGui/Toolbar/Main/ImGuiMainToolbar.h"
#include "editor/ImGui/Window/View/PrefabView/ImGuiPrefabView.h"
#include "editor/ImGui/Window/Statistics/ImGuiStatistics.h"

using namespace vg::core;
using namespace vg::editor;
using namespace ImGui;

#define VG_EDITOR_VERSION_MAJOR 0
#define VG_EDITOR_VERSION_MINOR 14

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
        m_imGuiWindows.push_back(new ImGuiPlatform());
        m_imGuiWindows.push_back(new ImGuiShader());
        m_imGuiWindows.push_back(new ImGuiFPS());
        m_imGuiWindows.push_back(new ImGuiResource());
        m_imGuiWindows.push_back(new ImGuiWorld());
        m_imGuiWindows.push_back(new ImGuiPrefab());
        m_imGuiWindows.push_back(new ImGuiInspector());
        m_imGuiWindows.push_back(new ImGuiEditorOptions());
        m_imGuiWindows.push_back(new ImGuiRendererOptions());
        m_imGuiWindows.push_back(new ImGuiPhysicsOptions());
        m_imGuiWindows.push_back(new ImGuiEngineOptions());        
        m_imGuiWindows.push_back(new ImGuiGameView(0));
        m_imGuiWindows.push_back(new ImGuiEditorView(0));
        m_imGuiWindows.push_back(new ImGuiConsole());
        m_imGuiWindows.push_back(new ImGuiInput());
        m_imGuiWindows.push_back(new ImGuiAbout());  
        m_imGuiWindows.push_back(new ImGuiStatistics());
        // Add ImGui toolbars
        m_imGuiWindows.push_back(new ImGuiMainToolbar("Main Toolbar", ImGuiWindow::StartVisible));

        // Plugins
        m_imGuiWindows.push_back(new ImGuiPlugin(IconWithText(style::icon::Plugin, "Plugin")));
	}

	//--------------------------------------------------------------------------------------
    Editor::~Editor()
	{
        for (uint i = 0; i < m_imGuiWindows.count(); ++i)
            VG_SAFE_RELEASE(m_imGuiWindows[i]);
        m_imGuiWindows.clear();
	}

    //--------------------------------------------------------------------------------------
    IEditorOptions * Editor::GetOptions() const
    {
        return EditorOptions::get();
    }

    //--------------------------------------------------------------------------------------
    bool Editor::RegisterClasses()
    {
        core::IFactory * factory = Kernel::getFactory();

        // Register classes to auto-register from the "Editor" module
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
    void ImGuizmoBeginFrame()
    {
        if (GImGui)
            ImGuizmo::BeginFrame();
    }

	//--------------------------------------------------------------------------------------
	void Editor::Init(const core::Singletons & _singletons)
	{
        // Get existing Singletons
        Kernel::setSingletons(_singletons);

        RegisterClasses();

        auto options = new EditorOptions("Editor Options", this);

        // Register ImGuizmo callback
        getRenderer()->GetImGuiAdapter()->AddBeginFrameCallback(ImGuizmoBeginFrame);
	}

	//--------------------------------------------------------------------------------------
	void Editor::Deinit()
	{
        auto * options = EditorOptions::get();
        VG_SAFE_DELETE(options);

        UnregisterClasses();
	}

    //--------------------------------------------------------------------------------------
    template <class T> T * Editor::getWindow(const core::string _name) const
    {
        for (uint i = 0; i < m_imGuiWindows.count(); ++i)
        {
            if (dynamic_cast<T *>(m_imGuiWindows[i]) != nullptr && (_name.empty() || m_imGuiWindows[i]->getName()._Starts_with(_name) ))
                return (T *)(m_imGuiWindows[i]);
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    template <class T> core::vector<T *> Editor::getWindows(const core::string _name) const
    {
        core::vector<T *> windows;
        for (uint i = 0; i < m_imGuiWindows.count(); ++i)
        {
            if (dynamic_cast<T*>(m_imGuiWindows[i]) != nullptr && (_name.empty() || m_imGuiWindows[i]->getName()._Starts_with(_name)))
                windows.push_back((T*)(m_imGuiWindows[i]));
        }

        return windows;
    }

    //--------------------------------------------------------------------------------------
    void Editor::destroyWindow(ImGuiWindow * _window)
    {
        m_imGuiWindowsToDestroy.push_back(_window);
    }

    //--------------------------------------------------------------------------------------
    vg::core::IFactory * Editor::getFactory() const
    {
        return Kernel::getFactory();
    }

    //--------------------------------------------------------------------------------------
    core::ISelection * Editor::getSelection() const
    {
        return getEngine()->GetSelection();
    }

    //--------------------------------------------------------------------------------------
    vg::engine::IEngine * Editor::getEngine() const
    {
        return (vg::engine::IEngine *)getFactory()->getSingleton("Engine");
    }

    //--------------------------------------------------------------------------------------
    vg::renderer::IRenderer * Editor::getRenderer() const
    {
        return (vg::renderer::IRenderer *)getFactory()->getSingleton("Renderer");
    }

    //--------------------------------------------------------------------------------------
    void Editor::RunOneFrame(float _dt)
    {
        VG_PROFILE_CPU("Editor");

        auto input = Kernel::getInput();
        auto options = EditorOptions::get();

        const bool shift = input->IsKeyPressed(Key::LSHIFT) || input->IsKeyPressed(Key::RSHIFT);
        const bool ctrl = input->IsKeyPressed(Key::LCONTROL) || input->IsKeyPressed(Key::RCONTROL);

        if (ctrl && !shift)
        {
            if (input->IsKeyJustPressed(Key::T))
                options->setGizmoType(GizmoType::Translate);
            else if (input->IsKeyJustPressed(Key::R))
                options->setGizmoType(GizmoType::Rotate);
            else if (input->IsKeyJustPressed(Key::S))
                options->setGizmoType(GizmoType::Scale);
            else if (input->IsKeyJustPressed(Key::A))
                options->setSnap(options->getSnap());
        }

        // Update Editor views cameras
        //auto * renderer = Editor::get()->getRenderer();
        //auto views = renderer->GetViews(gfx::ViewTarget::Editor);
        //for (uint i = 0; i < views.size(); ++i)
        //{
        //    auto view = views[i];
        //    if (view)
        //        view->updateEditorCamera();
        //}
    }

    //--------------------------------------------------------------------------------------
    void drawMenuItemRecur(ImGuiWindow * window, vector<string> folders, int _depth = 0)
    {
        if (_depth == folders.size())
        {
            if (ImGui::MenuItem(window->getIconizedName().c_str()))
                window->setVisible(true);
        }
        else
        {
            if (ImGui::BeginMenu(folders[_depth].c_str()))
            {
                drawMenuItemRecur(window, folders, _depth + 1);
                ImGui::EndMenu();
            }
        }
    };

	//--------------------------------------------------------------------------------------
	void Editor::DrawGUI(const GUIContext & _context)
	{
        VG_PROFILE_CPU("DrawGUI");

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
        static bool showImGuiDemo = false;

        core::IResource * worldRes = getEngine()->GetWorldResource();

        bool createNewWorld = false, loadWorld = false, saveWorld = false, saveWorldAs = false, closeWorld = false;

        ImGui::Begin("EditorDockSpace", &showUI, windowFlags);
        {
            ImGui::PopStyleVar(3);

            ImGuiID dockspaceId = ImGui::GetID("EditorDockSpace");
            ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);

            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::BeginMenu("World"))
                    {
                        if (ImGui::MenuItem("New"))
                            createNewWorld = true;

                        if (ImGui::MenuItem("Open"))
                            loadWorld = true;

                        ImGui::BeginDisabled(!worldRes || !worldRes->getObject());
                        {
                            if (ImGui::MenuItem("Save"))
                                saveWorld = true;
                        }
                        ImGui::EndDisabled();

                        ImGui::BeginDisabled(!worldRes || !worldRes->getObject());
                        {
                            if (ImGui::MenuItem("Save As"))
                                saveWorldAs = true;
                        }
                        ImGui::EndDisabled();

                        ImGui::Separator();

                        ImGui::BeginDisabled(!worldRes || !worldRes->getObject());
                        {
                            if (ImGui::MenuItem("Close"))
                                closeWorld = true;
                        }
                        ImGui::EndDisabled();

                        ImGui::EndMenu();
                    }

                    if (ImGui::MenuItem("Quit"))
                        ImGui::MessageBox(MessageBoxType::YesNo, "Quit", "Are you sure you want to quit program?", []() { Editor::get()->getEngine()->Quit(); return true; });

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Window"))
                {
                    auto sortedWindows = m_imGuiWindows;

                    sort(sortedWindows.begin(), sortedWindows.end(), [](ImGuiWindow * a, ImGuiWindow * b) 
                    {
                            if (a->getPath().empty() == b->getPath().empty())
                                return a->getName() < b->getName();
                            else if (a->getPath().empty())
                                return true;
                            else
                                return false;
                    }
                    );

                    for (uint i = 0; i < sortedWindows.count(); ++i)
                    {
                        auto window = sortedWindows[i];
                        if (asBool(window->getFlags() & ImGuiWindow::Flags::AddMenuEntry))
                        {
                            string folder = window->getPath();
                            if (!folder.empty())
                            {
                                vector<string> folders;

                                auto dirPos = folder.find_first_of("/");
                                if (string::npos != dirPos)
                                {
                                    auto temp = folder.substr(0, dirPos);
                                    folders.push_back(temp);
                                    folder = folder.substr(dirPos+1);
                                }

                                folders.push_back(folder);
                                drawMenuItemRecur(window, folders);                               
                            }
                            else
                            {
                                if (ImGui::MenuItem(window->getIconizedName().c_str()))
                                    window->setVisible(true);
                            }
                        }
                    }

                    if (ImGui::BeginMenu("View"))
                    {
                        // Get Prefab views
                        //vector<ImGuiPrefabView*> prefabWindows;
                        //for (uint i = 0; i < m_imGuiWindows.size(); ++i)
                        //{
                        //    if (auto * prefabView = dynamic_cast<ImGuiPrefabView *>(m_imGuiWindows[i]))
                        //        prefabWindows.push_back(prefabView);
                        //}
                        //
                        //ImGui::BeginDisabled(prefabWindows.size() == 0);
                        //{
                        //    if (ImGui::BeginMenu("Prefabs"))
                        //    {
                        //        for (uint p = 0; p < prefabWindows.size(); ++p)
                        //        {
                        //            auto window = prefabWindows[p];
                        //            if (ImGui::MenuItem(window->getName().c_str()))
                        //                window->setVisible(true);
                        //        }
                        //        ImGui::EndMenu();
                        //    }
                        //}
                        //ImGui::EndDisabled();
                        //ImGui::EndMenu();

                        auto world = getEngine()->GetMainWorld();
                        uint prefabCount = 0;
                        if (world)
                            prefabCount = world->GetSceneCount(BaseSceneType::Prefab);

                        ImGui::BeginDisabled(prefabCount == 0);
                        {
                            if (ImGui::BeginMenu("Prefabs"))
                            {
                                for (uint p = 0; p < prefabCount; ++p)
                                {
                                    auto prefab = world->GetScene(p, BaseSceneType::Prefab);
                                    if (ImGui::MenuItem(prefab->getName().c_str()))
                                        openPrefabView(prefab);
                                }
                                ImGui::EndMenu();
                            }
                        }
                        ImGui::EndDisabled();
                        ImGui::EndMenu();
                    }

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Options"))
                {
                    if (ImGui::IconMenuItem(style::icon::Editor, "Editor"))
                    {
                        ImGuiEditorOptions * options = getWindow<ImGuiEditorOptions>();
                        if (nullptr != options)
                            options->setVisible(true);
                    }

                    if (ImGui::IconMenuItem(style::icon::Engine, "Engine"))
                    {
                        ImGuiEngineOptions * options = getWindow<ImGuiEngineOptions>();
                        if (nullptr != options)
                            options->setVisible(true);
                    }

                    if (ImGui::IconMenuItem(style::icon::Renderer, "Renderer"))
                    {
                        ImGuiRendererOptions * options = getWindow<ImGuiRendererOptions>();
                        if (nullptr != options)
                            options->setVisible(true);
                    }

                    if (ImGui::IconMenuItem(style::icon::Physics, "Physics"))
                    {
                        ImGuiPhysicsOptions * options = getWindow<ImGuiPhysicsOptions>();
                        if (nullptr != options)
                            options->setVisible(true);
                    }

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Help"))
                {
                    if (ImGui::IconMenuItem(style::icon::Plugin, "Plugins"))
                    {
                        auto plugins = getWindow<ImGuiPlugin>();
                        if (nullptr != plugins)
                            plugins->setVisible(true);
                    }

                    ImGui::Separator();

                    if (ImGui::IconMenuItem(style::icon::Sliders, "ImGui demo"))
                        showImGuiDemo = true;

                    ImGui::Separator();

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

        for (auto & window : m_imGuiWindowsToDestroy)
        {
            m_imGuiWindows.remove(window);
            VG_SAFE_RELEASE_ASYNC(window);            
        }
        m_imGuiWindowsToDestroy.clear();

        string newWorldPopupName = "New World";
        string openFilePopupName = "Open World";
        string saveFilePopupName = "Save World";
        string saveAsFilePopupName = "Save World As";

        auto & fileBrowser = ImGuiWindow::getFileBrowser();
        string ext = ImGuiWindow::getFileBrowserExt(worldRes);

        const auto worldFolder = ImGuiWindow::getDefaultFolder("WorldResource");
        auto engine = getEngine();

        if (createNewWorld)
        {
            fileBrowser.setFolder(worldFolder);
            ImGui::OpenPopup(newWorldPopupName.c_str());
        }
        else if (loadWorld)
        {
            fileBrowser.setFolder(worldFolder);
            ImGui::OpenPopup(openFilePopupName.c_str());
        }
        else if (saveWorldAs)
        {
            fileBrowser.setFolder(worldFolder);
            ImGui::OpenPopup(saveAsFilePopupName.c_str());
        }
        else if (saveWorld)
        {
            engine->SaveWorld();
        }
        else if (closeWorld)
        {
            if (worldRes)
                worldRes->ClearResourcePath();
        }

        for (uint i = 0; i < m_imGuiWindows.size(); ++i)
        {
            if (m_imGuiWindows[i]->isVisible())
            {
                VG_PROFILE_CPU(m_imGuiWindows[i]->getName().c_str());
                m_imGuiWindows[i]->DrawGUI();
            }
        }

        // Create new file
        if (fileBrowser.showFileDialog(newWorldPopupName.c_str(), imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, style::dialog::Size, ext.c_str()))
        {
            // Close all prefab views when creating new world
            auto prefabViews = getWindows<ImGuiPrefabView>();
            for (auto * prefabView : prefabViews)
                prefabView->m_isVisible = false;

            engine->CreateWorld(io::addExtensionIfNotPresent(fileBrowser.selected_path, ".world"));
        }

        // Open existing file
        if (fileBrowser.showFileDialog(openFilePopupName.c_str(), imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, style::dialog::Size, ext.c_str()))
        {
            // Close all prefab views when loading new world
            auto prefabViews = getWindows<ImGuiPrefabView>();
            for (auto * prefabView : prefabViews)
                prefabView->m_isVisible = false;

            engine->LoadWorld(fileBrowser.selected_path);
        }

        // Save as 
        if (fileBrowser.showFileDialog(saveAsFilePopupName.c_str(), imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, style::dialog::Size, ext.c_str()))
            engine->SaveWorldAs(io::addExtensionIfNotPresent(fileBrowser.selected_path, ".world"));

        if (showImGuiDemo)
            ImGui::ShowDemoWindow(&showImGuiDemo);
    }    

    //--------------------------------------------------------------------------------------
    void Editor::openPrefabView(const core::IBaseScene * _prefab)
    {
        // Create or show window
        auto * prefabView = getWindow<ImGuiPrefabView>(_prefab->getName());

        if (prefabView)
        {
            prefabView->setVisible(true);
        }
        else
        {
            auto prefabView = new ImGuiPrefabView((IBaseScene *)_prefab);
            prefabView->setName(_prefab->getName());
            Editor::get()->m_imGuiWindows.push_back(prefabView);
        }
    }
}