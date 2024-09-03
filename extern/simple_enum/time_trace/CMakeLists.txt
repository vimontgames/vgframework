add_executable(simple_enum_tt )
target_sources(simple_enum_tt PRIVATE simple_enum.cc )
target_link_libraries(simple_enum_tt PRIVATE simple_enum::simple_enum)

add_executable(simple_enum_enum_cast_tt )
target_sources(simple_enum_enum_cast_tt PRIVATE enum_cast_tt.cc )
target_link_libraries(simple_enum_enum_cast_tt PRIVATE simple_enum::simple_enum)
target_compile_definitions(simple_enum_enum_cast_tt PRIVATE USE_SIMPLE_ENUM)


add_executable(magic_enum_enum_cast_tt )
target_sources(magic_enum_enum_cast_tt PRIVATE enum_cast_tt.cc )
target_link_libraries(magic_enum_enum_cast_tt PRIVATE simple_enum::simple_enum)
target_compile_definitions(magic_enum_enum_cast_tt PRIVATE USE_MAGIC_ENUM)


add_executable(magic_enum_tt)
target_sources(magic_enum_tt PRIVATE magic_enum.cc )
target_link_libraries(magic_enum_tt PRIVATE magic_enum::magic_enum)

add_executable(reflect_tt)
target_sources(reflect_tt PRIVATE reflect.cc )
target_link_libraries(reflect_tt PRIVATE reflect::reflect)
