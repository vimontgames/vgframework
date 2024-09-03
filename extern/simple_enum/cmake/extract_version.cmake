# extract_version.cmake

file(READ include/simple_enum/core.hpp simple_enum_hpp_content)
string(REGEX MATCH "SIMPLE_ENUM_NAME_VERSION \"([^\"]*)\"" _ ${simple_enum_hpp_content})
set(SIMPLE_ENUM_VERSION ${CMAKE_MATCH_1})


