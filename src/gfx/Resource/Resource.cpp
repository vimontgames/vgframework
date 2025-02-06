#include "gfx/Precomp.h"
#include "core/string/string.h"
#include "core/IScheduler.h"
#include "gfx/Device/Device.h"
#include "gfx/Memory/MemoryAllocator.h"
#include "Resource.h"
#include "Buffer.h"
#include "Texture.h"
#include "gfx/UploadBuffer/UploadBuffer.h"
#include "gfx/bindlessTable/BindlessTable.h"

#ifdef VG_DX12
#include "dx12/Resource_dx12.hpp"
#elif defined(VG_VULKAN)
#include "vulkan/Resource_vulkan.hpp"
#else
#error Undefined GFXAPI
#endif

#include "Texture.hpp"
#include "Buffer.hpp"