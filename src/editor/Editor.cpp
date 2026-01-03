#include "editor/Precomp.h"
#include "editor.h"
#include "core/IInput.h"
#include "core/IUndoRedo.h"
#include "core/IResource.h"
#include "core/IGameObject.h"
#include "core/File/File.h"
#include "core/Timer/Timer.h"
#include "core/UndoRedo/UndoRedo.h"
#include "renderer/IUIRenderer.h"
#include "renderer/IRenderer.h"
#include "renderer/IImGuiAdapter.h"
#include "engine/IEngine.h"
#include "engine/IWorldResource.h"
#include "editor/ImGui/Extensions/ImGuizmo/ImGuizmoAdapter.h"
#include "editor/Options/EditorOptions.h"
#include "editor/ImGui/Menu/ObjectHandle/ObjectHandleMenu.h"

#if !VG_ENABLE_INLINE
#include "Editor.inl"
#endif

#include "editor/ImGui/Extensions/imguiExtensions.h"
#include "editor/imgui/Extensions/FileDialog/ImGuiFileDialog.h"
#include "editor/ImGui/Window/FPS/ImGuiFPS.h"
#include "editor/ImGui/Window/Plugin/ImGuiPlugin.h"
#include "editor/ImGui/Window/Platform/ImGuiPlatform.h"
#include "editor/ImGui/Window/Inspector/ImGuiInspector.h"
#include "editor/ImGui/Window/Options/Editor/ImGuiEditorOptions.h"
#include "editor/ImGui/Window/Options/Engine/ImGuiEngineOptions.h"
#include "editor/ImGui/Window/Options/Renderer/ImGuiRendererOptions.h"
#include "editor/ImGui/Window/Options/Physics/ImGuiPhysicsOptions.h"
#include "editor/ImGui/Window/Options/Audio/ImGuiAudioOptions.h"
#include "editor/ImGui/Window/Options/Game/ImGuiGameOptions.h"
#include "editor/ImGui/Window/About/ImGuiAbout.h"
#include "editor/ImGui/Window/Shader/ImGuiShader.h"
#include "editor/ImGui/Window/SceneList/World/ImGuiWorld.h"
#include "editor/ImGui/Window/SceneList/Prefab/ImGuiPrefab.h"
#include "editor/ImGui/Window/Resource/ImGuiResource.h"
#include "editor/ImGui/Window/View/EditorView/ImGuiEditorView.h"
#include "editor/ImGui/Window/View/GameView/ImGuiGameView.h"
#include "editor/ImGui/Window/Console/ImGuiConsole.h"
#include "editor/ImGui/Window/Input/ImGuiInput.h"
#include "editor/ImGui/Window/Time/ImGuiTime.h"
#include "editor/ImGui/Toolbar/Main/ImGuiMainToolbar.h"
#include "editor/ImGui/Window/View/PrefabView/ImGuiPrefabView.h"
#include "editor/ImGui/Window/Statistics/ImGuiStatistics.h"
#include "editor/ImGui/Window/UndoRedo/ImGuiUndoRedo.h"
#include "editor/ImGui/ObjectHandler/ImGuiObjectHandler.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"

using namespace vg::core;
using namespace vg::editor;
using namespace ImGui;

//--------------------------------------------------------------------------------------
IEditor * CreateNew()
{
	return new Editor("Editor", nullptr);
}

//--------------------------------------------------------------------------------------
namespace vg::editor
{
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
        
	}

	//--------------------------------------------------------------------------------------
    Editor::~Editor()
	{
        for (auto i = 0; i < m_imGuiWindows.size(); ++i)
            VG_SAFE_RELEASE(m_imGuiWindows[i]);
        m_imGuiWindows.clear();

        for (auto i = 0; i < m_imGuiMenus.size(); ++i)
            VG_SAFE_RELEASE(m_imGuiMenus[i]);
        m_imGuiMenus.clear();

        // ImGui handlers must be destroyed before ImGui shutdown in case they perform ImGui allocs (e.g. ImGuiTextFilter)
        ImGuiObjectHandler::Destroy();
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
        return AutoRegisterClassInfo::unregisterClasses();
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

        VG_PROFILE_CPU("Editor");

        // Cache ptr to IEngine and IRenderer as Editor is going to need them very often ...
        m_engine = findEngine();
        m_renderer = findRenderer();

        Timer::init();

        RegisterClasses();

        auto options = new EditorOptions("Editor Options", this);

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
        m_imGuiWindows.push_back(new ImGuiAudioOptions());
        m_imGuiWindows.push_back(new ImGuiGameOptions());
        m_imGuiWindows.push_back(new ImGuiPhysicsOptions());
        m_imGuiWindows.push_back(new ImGuiEngineOptions());
        m_imGuiWindows.push_back(new ImGuiGameView(0));
        m_imGuiWindows.push_back(new ImGuiEditorView(0));
        m_imGuiWindows.push_back(new ImGuiConsole());
        m_imGuiWindows.push_back(new ImGuiInput());
        m_imGuiWindows.push_back(new ImGuiAbout());
        m_imGuiWindows.push_back(new ImGuiStatistics());
        m_imGuiWindows.push_back(new ImGuiTime());
        m_imGuiWindows.push_back(new ImGuiUndoRedo());

        // Add ImGui toolbars
        m_imGuiWindows.push_back(new ImGuiMainToolbar("Main Toolbar", ImGuiWindow::StartVisible));

        // Plugins
        m_imGuiWindows.push_back(new ImGuiPlugin(IconWithText(style::icon::Plugin, "Plugin")));

        // Menus
        m_imGuiMenus.push_back(new ImGuiObjectHandleMenu());

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
        for (auto i = 0; i < m_imGuiWindows.size(); ++i)
        {
            if (dynamic_cast<T *>(m_imGuiWindows[i]) != nullptr && (_name.empty() || m_imGuiWindows[i]->GetName()._Starts_with(_name) ))
                return (T *)(m_imGuiWindows[i]);
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    template <class T> core::vector<T *> Editor::getWindows(const core::string _name) const
    {
        core::vector<T *> windows;
        for (auto i = 0; i < m_imGuiWindows.size(); ++i)
        {
            if (dynamic_cast<T*>(m_imGuiWindows[i]) != nullptr && (_name.empty() || m_imGuiWindows[i]->GetName()._Starts_with(_name)))
                windows.push_back((T*)(m_imGuiWindows[i]));
        }

        return windows;
    }

    //--------------------------------------------------------------------------------------
    core::vector<ImGuiWindow *> Editor::GetWindows(const core::string & _className) const
    {
        core::vector<ImGuiWindow *> windows;
        for (auto i = 0; i < m_imGuiWindows.size(); ++i)
        {
            auto & win = m_imGuiWindows[i];
            const char * className = win->GetClassName();
            if (!strcmp(className, _className.c_str()))
                windows.push_back(m_imGuiWindows[i]);
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
    vg::engine::IEngine * Editor::findEngine() const
    {
        return (vg::engine::IEngine *)getFactory()->GetSingleton("Engine");
    }

    //--------------------------------------------------------------------------------------
    vg::renderer::IRenderer * Editor::findRenderer() const
    {
        return (vg::renderer::IRenderer *)getFactory()->GetSingleton("Renderer");
    }

    //--------------------------------------------------------------------------------------
    void Editor::RunOneFrame()
    {
        VG_PROFILE_CPU("Editor");

        auto input = Kernel::getInput();
        auto * options = EditorOptions::get();

        options->Update();

        const bool shift = input->IsKeyPressed(Key::LeftShift) || input->IsKeyPressed(Key::RightShift);
        const bool ctrl = input->IsKeyPressed(Key::LeftControl) || input->IsKeyPressed(Key::RightControl);

        auto wheel = input->GetMouseDelta().z;
        if (ctrl && wheel != 0)
        {
            static core::Ticks lastGizmoChangedTick = core::Timer::getTick();
            auto delay = Timer::getEnlapsedTime(lastGizmoChangedTick, Timer::getTick());

            if (delay > 100.0f) // 0.1 sec
            {
                if (wheel > 0)
                    options->setNextGizmo();
                else if (wheel < 0)
                    options->setPreviousGizmo();

                lastGizmoChangedTick = core::Timer::getTick();
            }
        }

        if (ctrl)
        {
            if (input->IsKeyJustPressed(Key::Z))
                Kernel::getUndoRedoManager()->Undo();

            else if (input->IsKeyJustPressed(Key::Y))
                Kernel::getUndoRedoManager()->Redo();
        }
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

        auto * imGuiContext = (ImGuiContext*)_context.imgui;
        ImGui::SetCurrentContext(imGuiContext);
        ImGuiFileDialog::Instance((ImGuiFileDialog *)_context.filedialog);

        ImGuiViewport * viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos, ImGuiCond_Appearing, ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(viewport->WorkSize, ImGuiCond_None);
        ImGui::SetNextWindowViewport(viewport->ID);

        const auto * renderer = getRenderer();
        const bool fullscreen = renderer->IsFullscreen();

        ImGuiWindowFlags windowFlags = 0x0;
        
        if (!fullscreen)
            windowFlags |= ImGuiWindowFlags_MenuBar;

        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        windowFlags |= ImGuiWindowFlags_NoBackground;

        ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

        if (fullscreen)
        {
            dockspaceFlags |= ImGuiDockNodeFlags_NoDockingOverCentralNode;
            windowFlags |= ImGuiWindowFlags_NoScrollbar;
        }

        bool showUI = true;
        static bool showImGuiDemo = false;

        core::IResource * worldRes = getEngine()->GetWorldResource();

        bool createNewWorld = false, loadWorld = false, saveWorld = false, saveWorldAs = false, closeWorld = false;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.5f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        const char * EditorDockSpace = "Game";

        ImGui::Begin(EditorDockSpace, &showUI, windowFlags);
        {
            ImGui::PopStyleVar(3);

            // Render game UI to backbuffer inside dockspace
            if (fullscreen)
            {
                auto viewports = renderer->GetViewports(gfx::ViewportTarget::Game);
                for (auto & viewport : viewports)
                {
                    for (auto & pair : viewport->GetViewIDs())
                    {
                        if (auto * view = renderer->GetView(pair.second))
                        {
                            if (auto * uiRenderer = view->GetUIRenderer())
                                uiRenderer->RenderFullscreen(renderer::RenderUIType::RenderUIType_3D);
                        }

                        // Render viewport GUI on top of view GUIs
                        if (auto * uiRenderer = viewport->GetUIRenderer())
                            uiRenderer->RenderFullscreen(renderer::RenderUIType::RenderUIType_2D);
                    }                 
                }
            }
            else
            {
                ImGuiID dockspaceId = ImGui::GetID(EditorDockSpace);
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

                            ImGui::BeginDisabled(!worldRes || !worldRes->GetObject());
                            {
                                if (ImGui::MenuItem("Save"))
                                    saveWorld = true;
                            }
                            ImGui::EndDisabled();

                            ImGui::BeginDisabled(!worldRes || !worldRes->GetObject());
                            {
                                if (ImGui::MenuItem("Save As"))
                                    saveWorldAs = true;
                            }
                            ImGui::EndDisabled();

                            ImGui::Separator();

                            ImGui::BeginDisabled(!worldRes || !worldRes->GetObject());
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
                                    return a->GetName() < b->GetName();
                                else if (a->getPath().empty())
                                    return true;
                                else
                                    return false;
                            }
                        );

                        for (auto i = 0; i < sortedWindows.size(); ++i)
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
                                        folder = folder.substr(dirPos + 1);
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
                            engine::IWorldResource * worldRes = getEngine()->GetWorldResource();

                            uint prefabCount = 0;
                            if (worldRes)
                                prefabCount = worldRes->GetSceneResourceCount(BaseSceneType::Prefab);

                            ImGui::BeginDisabled(prefabCount == 0);
                            {
                                if (ImGui::BeginMenu("Prefabs"))
                                {
                                    for (uint p = 0; p < prefabCount; ++p)
                                    {
                                        auto prefabRes = worldRes->GetSceneResource(p, BaseSceneType::Prefab);

                                        if (ImGui::MenuItem(io::getFileNameWithoutExt(prefabRes->GetResourcePath()).c_str()))
                                            openPrefabView(prefabRes);
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
            }

            ProcessMessageBox();
            ImGuiAxis axis = ImGuiAxis_X;
        }
        ImGui::End();

        for (auto & window : m_imGuiWindowsToDestroy)
        {
            vector_helper::remove(m_imGuiWindows, window);
            VG_SAFE_RELEASE_ASYNC(window);
        }
        m_imGuiWindowsToDestroy.clear();

        string newWorldPopupName = "New World";
        string openFilePopupName = "Open World";
        string saveFilePopupName = "Save World";
        string saveAsFilePopupName = "Save World As";

        string ext = ImGuiWindow::getFileBrowserExt(worldRes);

        const auto worldFolder = ImGuiWindow::getDefaultFolder("WorldResource");
        auto engine = getEngine();

        if (createNewWorld)
        {
            OpenFileDialog(newWorldPopupName, ext, worldFolder);
        }
        else if (loadWorld)
        {
            OpenFileDialog(openFilePopupName, ext, worldFolder);
        }
        else if (saveWorldAs)
        {
            SaveFileDialog(saveAsFilePopupName, ext, worldFolder);
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

        if (!fullscreen)
        {
            for (uint i = 0; i < m_imGuiWindows.size(); ++i)
            {
                if (m_imGuiWindows[i]->isVisible())
                {
                    VG_PROFILE_CPU(m_imGuiWindows[i]->GetName().c_str());
                    m_imGuiWindows[i]->DrawGUI();
                }
            }
        }

        // Create new ".world" file
        if (DisplayFileDialog(newWorldPopupName))
        {
            if (ImGui::IsFileDialogOK())
            {
                // Close all prefab views when creating new world
                auto prefabViews = getWindows<ImGuiPrefabView>();
                for (auto * prefabView : prefabViews)
                    prefabView->setVisible(false);

                engine->CreateWorld(io::addExtensionIfNotPresent(GetFileDialogSelectedFile(), ext));
            }

            ImGui::CloseFileDialog();
        }

        // Open existing file
        if (DisplayFileDialog(openFilePopupName))
        {
            if (ImGui::IsFileDialogOK())
            {
                // Close all prefab views when loading new world
                auto prefabViews = getWindows<ImGuiPrefabView>();
                for (auto * prefabView : prefabViews)
                    prefabView->setVisible(false);

                engine->LoadWorld(GetFileDialogSelectedFile());
            }

            ImGui::CloseFileDialog();
        }

        // Save as 
        if (DisplayFileDialog(saveAsFilePopupName))
        {
            if (ImGui::IsFileDialogOK())
            {
                // Close all prefab views when loading new world
                auto prefabViews = getWindows<ImGuiPrefabView>();
                for (auto * prefabView : prefabViews)
                    prefabView->setVisible(false);

                engine->SaveWorldAs(GetFileDialogSelectedFile());
            }

            ImGui::CloseFileDialog();
        }

        if (showImGuiDemo)
            ImGui::ShowDemoWindow(&showImGuiDemo);
    }    

    //--------------------------------------------------------------------------------------
    void Editor::openPrefabView(const core::IResource * _prefabRes)
    {
        // Create or show window
        auto * editor = Editor::get();
        auto * prefabView = getWindow<ImGuiPrefabView>(io::getFileNameWithoutExt(_prefabRes->GetResourcePath()));

        if (prefabView)
        {
            prefabView->setVisible(true);
        }
        else
        {
            prefabView = new ImGuiPrefabView(_prefabRes);
            prefabView->SetName(io::getFileNameWithoutExt(_prefabRes->GetResourcePath()));
            editor->m_imGuiWindows.push_back(prefabView);
        }

        if (prefabView)
            prefabView->setWindowFocus();
    }

    //--------------------------------------------------------------------------------------
    void Editor::focus(core::IGameObject * _gameObject)
    {
        core::vector<core::IGameObject *> gameObjects;
        gameObjects.push_back(_gameObject);
        focus(gameObjects);
    }

    //--------------------------------------------------------------------------------------
    void Editor::focus(const core::vector<core::IGameObject *> &_gameObjects)
    {
        if (_gameObjects.size() > 0)
        {
            IWorld * world = _gameObjects[0]->GetWorld();
            auto * scene = _gameObjects[0]->GetScene();

            for (uint i = 1; i < _gameObjects.size(); ++i)
            {
                VG_ASSERT(_gameObjects[i]->GetWorld() == world);
                VG_ASSERT(_gameObjects[i]->GetScene() == scene);
            }

            const auto sceneType = scene->GetSceneType();

            auto imGuiViews = getWindows<ImGuiView>();

            for (ImGuiView * view : imGuiViews)
            {
                switch (sceneType)
                {
                default:
                    break;

                case BaseSceneType::Scene:
                {
                    if (view->getViewportTarget() == gfx::ViewportTarget::Editor && view->getViewportIndex() == 0)  // TODO: ImGuiView::isMainEditorViewport()
                        view->focus(_gameObjects);
                }
                break;

                case BaseSceneType::Prefab:
                {
                    if (world == view->GetWorld())
                        view->focus(_gameObjects);
                }
                break;
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void Editor::deleteGameObjects(core::vector<IGameObject *> & _gameObjects)
    {
        ImGui::OnMsgBoxClickedFunc deleteGameObject = [=]() mutable
        {
            auto * undoRedoManager = Kernel::getUndoRedoManager();

            // Prepare undo/redo
            UndoRedoTarget undoRedoTarget(this, nullptr);

            auto * undoRedoGroup = new UndoRedoEntryGroup("Destroy");
            for (uint i = 0; i < _gameObjects.size(); ++i)
                undoRedoGroup->AddSubEntry(new UndoRedoDestroyEntry(_gameObjects[i], _gameObjects[i]->GetParent(), i));
            undoRedoManager->BeforeChange(undoRedoGroup);

            for (uint i = 0; i < _gameObjects.size(); ++i)
            {
                IGameObject * gameObjectToDelete = _gameObjects[i];
                IGameObject * parentGameObject = dynamic_cast<IGameObject *>(gameObjectToDelete->GetParent());
                if (nullptr != parentGameObject)
                    parentGameObject->RemoveChild(gameObjectToDelete);
            }

            // Finalize Undo/Redo entry after editing
            undoRedoManager->AfterChange();

            return true;
        };

        string msg;
        if (_gameObjects.size() > 1)
            msg = "Are you sure you want to delete " + to_string(_gameObjects.size()) + " GameObjects and their children?";
        else
            msg = "Are you sure you want to delete " + (string)_gameObjects[0]->GetClassName() + " \"" + _gameObjects[0]->GetName() + "\"?";
        ImGui::MessageBox(MessageBoxType::YesNo, "Delete GameObject", msg.c_str(), deleteGameObject);
    }
}