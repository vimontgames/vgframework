#pragma once

#ifdef VG_DEBUG
#define VG_OVERRIDE_ENABLE_GLOBAL_NEW_DELETE 0 // Override VG_ENABLE_GLOBAL_NEW_DELETE 
#endif

#include "core/Precomp.h"
#include "gtest/gtest.h"
