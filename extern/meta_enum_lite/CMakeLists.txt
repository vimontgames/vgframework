cmake_minimum_required(VERSION 3.12)
project(meta_enum)

set(CMAKE_CXX_STANDARD 17)

set(LIB_NAME ${PROJECT_NAME})

# Create target and add include path
add_library(${LIB_NAME} INTERFACE)
add_library(${LIB_NAME}::${LIB_NAME} ALIAS ${LIB_NAME})

# Add include directory to library
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(
    ${PROJECT_NAME}
    INTERFACE
    $<BUILD_INTERFACE:${INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)

