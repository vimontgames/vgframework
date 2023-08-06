#include "editor/Precomp.h"
#include "editor.h"
#include "renderer/IRenderer.h"
#include "imgui/imgui.h"

#if !VG_ENABLE_INLINE
#include "Editor.inl"
#endif

#include "imgui/imgui.cpp"
#include "imgui/imgui_demo.cpp"
#include "imgui/imgui_draw.cpp"
#include "imgui/imgui_tables.cpp"
#include "imgui/imgui_widgets.cpp"

using namespace vg::core;
using namespace vg::editor;

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
        
	}

	//--------------------------------------------------------------------------------------
    Editor::~Editor()
	{

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

        Kernel::setScheduler(nullptr);
        Kernel::setInput(nullptr);
        Kernel::setFactory(nullptr);
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
        }
        ImGui::End();

        static bool demo = false;
        if (demo)
            ImGui::ShowDemoWindow(&demo);
	}
}