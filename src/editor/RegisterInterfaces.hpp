#include "editor/Precomp.h"
#include "core/Object/AutoRegisterClass.h"

#include "IEditorOptions.h"

//--------------------------------------------------------------------------------------
// Register physics abstract classes
// 
// Each abstract interfaces has to be declared in an object file for RTTI
// Module abstract classes that have no associated .cpp file can be registered here
//--------------------------------------------------------------------------------------
namespace vg::editor
{
    VG_REGISTER_ABSTRACT_CLASS(IEditorOptions);
}
