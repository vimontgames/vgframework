#include "ImGui.h"

#pragma push_macro("new")
#undef new

#include "imgui/imgui.cpp"
#include "imgui/imgui_demo.cpp"
#include "imgui/imgui_draw.cpp"
#include "imgui/imgui_tables.cpp"
#include "imgui/imgui_widgets.cpp"

#ifdef _WIN32
#include "imgui/backends/imgui_impl_win32.cpp"
#endif

#ifdef VG_DX12
#include "imgui/backends/imgui_impl_dx12.cpp"
#elif defined(VG_VULKAN)
#include "imgui/backends/imgui_impl_vulkan.cpp"
#endif

#include "editor/ImGui/Extensions/FileDialog/ImGuiFileDialog.cpp"

#pragma pop_macro("new")