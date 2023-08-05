#include "editor/Precomp.h"

#include "editor.h"

#include "core/Kernel.h"
#include "core/IProfiler.h"
#include "core/Universe/Universe.h"
#include "core/Timer/Timer.h"
#include "core/Plugin/Plugin.h"
#include "core/Scheduler/Scheduler.h"
#include "core/Object/Factory.h"
#include "core/Scene/Scene.h"
#include "core/GameObject/GameObject.h"

#include "gfx/IView.h"
#include "gfx/IDevice.h"

#include "renderer/IRenderer.h"

#include "engine/Input/Input.h"
#include "engine/Resource/ResourceManager.h"
#include "engine/Component/Camera/CameraComponent.h"
#include "engine/Behaviour/FreeCam/FreeCamBehaviour.h"
#include "engine/Component/Mesh/MeshComponent.h"

//#include "editor/IEditor.h"

#include "application/IProject.h"

#if !VG_ENABLE_INLINE
#include "Editor.inl"
#endif

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
	void Editor::RunOneFrame()
	{
        VG_PROFILE_FRAME("Editor");
        VG_PROFILE_CPU("Editor");
	}
}