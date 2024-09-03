
add_ut_test(test_simple_enum.cc)
target_compile_definitions( test_simple_enum PRIVATE BOOST_UT_DISABLE_MODULE)

add_ut_test(ranges_views_ut.cc)
target_compile_definitions( ranges_views_ut PRIVATE BOOST_UT_DISABLE_MODULE)

add_ut_test(enum_cast_ut.cc)
target_compile_definitions( enum_cast_ut PRIVATE BOOST_UT_DISABLE_MODULE)

add_ut_test(enum_index_ut.cc)
target_compile_definitions( enum_index_ut PRIVATE BOOST_UT_DISABLE_MODULE)

add_ut_test(generic_error_category_ut.cc)
target_compile_definitions( generic_error_category_ut PRIVATE BOOST_UT_DISABLE_MODULE)

if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "13") OR
    (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "16") OR
    (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC"))
  add_ut_test(std_format_ut.cc)
endif()

CPMAddPackage(
  NAME fmt
  GITHUB_REPOSITORY fmtlib/fmt
  GIT_TAG 10.2.1
)

# find_package(fmt REQUIRED)
add_ut_test(fmtlib_format_ut.cc)
target_link_libraries(fmtlib_format_ut PRIVATE fmt::fmt)

add_ut_test(glaze_enum_name_ut.cc)

target_link_libraries(glaze_enum_name_ut PRIVATE glaze::glaze)

add_executable(diagnostics EXCLUDE_FROM_ALL)
target_sources( diagnostics PRIVATE diagnostics.cc)
target_compile_definitions( diagnostics PRIVATE BOOST_UT_DISABLE_MODULE)

