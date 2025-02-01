#include "editor/Precomp.h"
#include "ImGuiObjectHandler.h"
#include "core/IObject.h"
#include "editor/Editor_Consts.h"

namespace vg::editor
{
    core::vector<ImGuiObjectHandler::HandlerInfo> ImGuiObjectHandler::s_ImGuiHandlers;
}

#include "GameObject/ImGuiGameObjectHandler.hpp"
#include "Texture/ImGuiTextureHandler.hpp"
#include "Texture/ImGuiTextureResourceHandler.hpp"
#include "Material/ImGuiMaterialModelTypeHandler.hpp"
#include "Curve/ImGuiCurveObjectHandler.hpp"

#include "ImGuiResourceListHandler.hpp"

#include "Material/ImGuiMaterialListHandler.hpp"
#include "Animation/ImGuiAnimationListHandler.hpp"
#include "Animation/ImGuiSkeletonObjectHandler.hpp"
#include "Audio/ImGuiSoundListHandler.hpp"