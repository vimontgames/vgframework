#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS

#pragma push_macro("new")
#undef new
#include "../extern/imgui/imgui.h"
#include "imgui/imgui_internal.h"
#pragma pop_macro("new")