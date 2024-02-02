#pragma once
#pragma push_macro("new")
#undef new
#include "tinyxml2/tinyxml2.h"
#pragma pop_macro("new")

namespace vg::core
{
    using XMLDoc = tinyxml2::XMLDocument;
    using XMLElem = tinyxml2::XMLElement;
}