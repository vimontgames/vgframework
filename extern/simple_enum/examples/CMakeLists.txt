
add_executable(bounded_enums EXCLUDE_FROM_ALL bounded_enums.cc)
target_link_libraries(bounded_enums PRIVATE simple_enum::simple_enum)
  
add_executable(enum_cast EXCLUDE_FROM_ALL enum_cast.cc)
target_link_libraries(enum_cast PRIVATE simple_enum::simple_enum)

add_executable(enum_view EXCLUDE_FROM_ALL enum_view.cc)
target_link_libraries(enum_view PRIVATE simple_enum::simple_enum)

add_executable(generic_error_category EXCLUDE_FROM_ALL generic_error_category.cc)
target_link_libraries(generic_error_category PRIVATE simple_enum::simple_enum fmt::fmt)

add_executable(glaze_json_enum_support EXCLUDE_FROM_ALL glaze_json_enum_support.cc)

target_link_libraries(glaze_json_enum_support PRIVATE simple_enum::simple_enum glaze::glaze)
