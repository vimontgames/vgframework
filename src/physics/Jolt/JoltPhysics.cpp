#include "physics/Precomp.h"

#pragma push_macro("new")
#undef new

#include "Jolt/Math/Vec3.cpp"

#include "Jolt/Physics/DeterminismLog.cpp"
#include "Jolt/Physics/IslandBuilder.cpp"
#include "Jolt/Physics/LargeIslandSplitter.cpp"
#include "Jolt/Physics/PhysicsLock.cpp"
#include "Jolt/Physics/PhysicsScene.cpp"
#include "Jolt/Physics/PhysicsSystem.cpp"
#include "Jolt/Physics/PhysicsUpdateContext.cpp"
#include "Jolt/Physics/StateRecorderImpl.cpp"

#pragma pop_macro("new")