#include "graphics/driver/Precomp.h"
#include "core/string/string.h"
#include "graphics/driver/Device/Device.h"
#include "graphics/driver/Memory/MemoryAllocator.h"
#include "Resource.h"
#include "Buffer.h"
#include "Texture.h"
#include "graphics/driver/bindlessTable/BindlessTable.h"

using namespace vg::core;

#include VG_GFXAPI_IMPL(Resource)

#include "Texture.hpp"
#include "Buffer.hpp"