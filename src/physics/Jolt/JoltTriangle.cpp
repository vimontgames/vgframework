#include "physics/Precomp.h"

#pragma push_macro("new")
#undef new

#include "Jolt/TriangleGrouper/TriangleGrouperClosestCentroid.cpp"
#include "Jolt/TriangleGrouper/TriangleGrouperMorton.cpp"

#include "Jolt/TriangleSplitter/TriangleSplitter.cpp"
#include "Jolt/TriangleSplitter/TriangleSplitterBinning.cpp"
#include "Jolt/TriangleSplitter/TriangleSplitterFixedLeafSize.cpp"
#include "Jolt/TriangleSplitter/TriangleSplitterLongestAxis.cpp"
#include "Jolt/TriangleSplitter/TriangleSplitterMean.cpp"
#include "Jolt/TriangleSplitter/TriangleSplitterMorton.cpp"

#pragma pop_macro("new")