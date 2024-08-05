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

#pragma push_macro("new")
#undef new

#include "Jolt/Jolt.h"
#include "Jolt/Physics/PhysicsSystem.h"
#include "Jolt/Physics/Collision/Shape/Shape.h"
#include "Jolt/Physics/Collision/ObjectLayer.h"
#include "Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/Body/MotionType.h"
#include "Jolt/Physics/Character/Character.h"
#include "Jolt/Physics/Body/BodyID.h"
#include "Jolt/Core/JobSystemWithBarrier.h"
#include "Jolt/RegisterTypes.h"
#include "Jolt/Core/Factory.h"
#include <Jolt/Physics/Body/BodyManager.h>
#include "Jolt/Physics/Collision/Shape/StaticCompoundShape.h"
#include "Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h"
#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"
#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "Physics/Helper.h"

#pragma pop_macro("new")
