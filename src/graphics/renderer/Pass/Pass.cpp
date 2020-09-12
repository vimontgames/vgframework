#include "graphics/renderer/Precomp.h"

#include "core/Math/Math.h"

#include "graphics/driver/Device/Device.h"
#include "graphics/driver/CommandList/CommandList.h"
#include "graphics/driver/Texture/Texture.h"
#include "graphics/driver/PipelineState/RasterizerState.h"
#include "graphics/driver/RootSignature/RootSignature.h"
#include "graphics/driver/Shader/ShaderManager.h"
#include "graphics/driver/Shader/Shader.h"
#include "graphics/renderer/Renderer.h"

#include "TestPass.h"

using namespace vg::core;
using namespace vg::graphics::driver;

#include "TestPass.hpp"