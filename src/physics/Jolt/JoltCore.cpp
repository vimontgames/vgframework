#include "physics/Precomp.h"

#pragma push_macro("new")
#undef new

#include "Jolt/Core/Color.cpp"
#include "Jolt/Core/Factory.cpp"
#include "Jolt/Core/IssueReporting.cpp"
#include "Jolt/Core/JobSystemSingleThreaded.cpp"
#include "Jolt/Core/JobSystemThreadPool.cpp"
#include "Jolt/Core/JobSystemWithBarrier.cpp"
#include "Jolt/Core/LinearCurve.cpp"
#include "Jolt/Core/Memory.cpp"
#include "Jolt/Core/Profiler.cpp"
#include "Jolt/Core/RTTI.cpp"
#include "Jolt/Core/Semaphore.cpp"
#include "Jolt/Core/StringTools.cpp"
#include "Jolt/Core/TickCounter.cpp"

#pragma pop_macro("new")