#include "core/Precomp.h"
#include "Math.h"

namespace vg::core
{
	void test()
	{
		float4 foo4 = float4(1, 2, 3, 4);
		float3 foo3 = foo4.zyw;
		float2 foo2 = foo3.xz + foo3.zz;
	}
}