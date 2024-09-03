
# Installation Guide for simple_enum using CPM.cmake

This guide will walk you through the steps to integrate `simple_enum` into your C++ project using CPM.cmake, a C++ Package Manager.

## Prerequisites

`CPM.cmake` is required, but if you don't have it set up in your project yet, the installation steps will cover its integration.

## Step 1: Set Up CPM.cmake

If you haven't already integrated CPM.cmake into your project, this script will download get_cpm:

```bash
mkdir -p cmake
wget -O cmake/CPM.cmake https://github.com/cpm-cmake/CPM.cmake/releases/latest/download/get_cpm.cmake

```

This snippet will download and make CPM.cmake available in your project.

## Step 2: Adding simple_enum with CPM.cmake

Now, to add `simple_enum` to your project, include the following `CPMAddPackage` command in your `CMakeLists.txt` file:

```cmake
include(cmake/CPM.cmake)

CPMAddPackage(
  NAME simple_enum
  GITHUB_REPOSITORY arturbac/simple_enum
  GIT_TAG v0.5.7
)
```

This command instructs CPM.cmake to fetch the `simple_enum` library from the specified GitHub repository and tag.

## Step 3: Optional Precompiled Headers (PCH) Setup

To speed up compilation times, you can optionally precompile the headers of `simple_enum`. Add the following to your `CMakeLists.txt` after including `simple_enum`:

```cmake
target_precompile_headers(simple_enum INTERFACE
  "${simple_enum_SOURCE_DIR}/simple_enum/simple_enum.hpp"
  "${simple_enum_SOURCE_DIR}/simple_enum/std_format.hpp"
  "${simple_enum_SOURCE_DIR}/simple_enum/enum_cast.hpp"
  "${simple_enum_SOURCE_DIR}/simple_enum/enum_index.hpp"
  "${simple_enum_SOURCE_DIR}/simple_enum/ranges_views.hpp"
  "${simple_enum_SOURCE_DIR}/simple_enum/fmtlib_format.hpp"
)
```

This step is optional for projects with significant compilation times.

## Step 4: Using simple_enum in Your Project

After the setup, you can use `simple_enum` in your project. Just include the necessary headers in your source files:

```cpp
#include "simple_enum/simple_enum.hpp"
// Include other headers as needed
```

You can now leverage `simple_enum`'s functionality in your C++ project.
