#include "editor/Precomp.h"
#include "editor.h"
#include "core/Kernel.h"

#if !VG_ENABLE_INLINE
#include "Editor.inl"
#endif

#define VG_EDITOR_VERSION_MAJOR 0
#define VG_EDITOR_VERSION_MINOR 1

using namespace vg::core;
using namespace vg::editor;

//--------------------------------------------------------------------------------------
IEditor * CreateNew()
{
	return new Editor("Engine", nullptr);
}

//--------------------------------------------------------------------------------------
namespace vg::editor
{
	//--------------------------------------------------------------------------------------
	IPlugin::Version Editor::getVersion() const
	{
		return { VG_EDITOR_VERSION_MAJOR, VG_EDITOR_VERSION_MINOR };
	}

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
    bool Editor::registerClasses()
    {
        core::IFactory * factory = Kernel::getFactory();

        AutoRegisterClassInfo::registerClasses(*factory);

        if (core::IClassDesc * desc = factory->registerClassSingletonHelper(Editor, "Editor", IClassDesc::Flags::None))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Editor::unregisterClasses()
    {
        IFactory * factory = Kernel::getFactory();
        return AutoRegisterClassInfo::unregisterClasses(*factory);
    }


    //--------------------------------------------------------------------------------------
    bool Editor::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }    

	//--------------------------------------------------------------------------------------
	void Editor::Init()
	{
        
	}

	//--------------------------------------------------------------------------------------
	void Editor::Deinit()
	{
        
	}

	//--------------------------------------------------------------------------------------
	void Editor::RunOneFrame()
	{
        VG_PROFILE_FRAME("Editor");
        VG_PROFILE_CPU("Editor");
	}	
}