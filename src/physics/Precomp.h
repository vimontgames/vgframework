#include "core/Precomp.h"
#include "core/IProfiler.h"
#include "core/Object/AutoRegisterClass.h"

#if defined(VG_DEBUG) || defined(VG_RELEASE)
#define JPH_PROFILE_ENABLED
#define JPH_DEBUG_RENDERER
#define JPH_FLOATING_POINT_EXCEPTIONS_ENABLED
#endif

#define JPH_USE_AVX2
#define JPH_USE_AVX
#define JPH_USE_SSE4_1
#define JPH_USE_SSE4_2
#define JPH_USE_LZCNT
#define JPH_USE_TZCNT
#define JPH_USE_F16C
#define JPH_USE_FMADD

#include "Jolt/Jolt.h"
#include "Jolt/Physics/PhysicsSystem.h"
#include "Jolt/Physics/Collision/Shape/Shape.h"
#include "Jolt/Physics/Collision/ObjectLayer.h"
#include "Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/Body/MotionType.h"
#include "Jolt/Physics/Character/Character.h"
