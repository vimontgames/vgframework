#include "gfx/Precomp.h"
#include "core/string/string.h"
#include "gfx/Device/Device.h"
#include "gfx/Memory/MemoryAllocator.h"
#include "Resource.h"
#include "Buffer.h"
#include "Texture.h"
#include "UploadBuffer.h"
#include "gfx/bindlessTable/BindlessTable.h"

using namespace vg::core;

#include VG_GFXAPI_IMPL(Resource)

#include "Texture.hpp"
#include "Buffer.hpp"
#include "UploadBuffer.hpp"