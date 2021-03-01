#include "graphics/renderer/Precomp.h"

#include "core/Math/Math.h"

#include "graphics/driver/Device/Device.h"
#include "graphics/driver/CommandList/CommandList.h"
#include "graphics/driver/Resource/Texture.h"
#include "graphics/driver/Resource/Buffer.h"
#include "graphics/driver/Resource/Resource_consts.h"
#include "graphics/driver/PipelineState/DepthStencilState.h"
#include "graphics/driver/PipelineState/RasterizerState.h"
#include "graphics/driver/PipelineState/BlendState.h"
#include "graphics/driver/RootSignature/RootSignature.h"
#include "graphics/driver/Shader/ShaderManager.h"
#include "graphics/driver/Shader/Shader.h"
#include "graphics/driver/BindlessTable/BindlessTable.h"
#include "graphics/driver/Profiler/Profiler.h"
#include "graphics/renderer/Renderer.h"

using namespace vg::core;
using namespace vg::graphics::driver;

#include "BackgroundPass.hpp"
#include "TestPass2D.hpp"
#include "TestPass3D.hpp"
#include "PostProcessPass.hpp"
#include "ImguiPass.hpp"