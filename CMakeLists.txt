# Grok Desktop - Windows C++20 + CEF client for https://grok.com/
#
# Follows the integration pattern documented in the official CEF binary
# distribution (third_party/cef/CMakeLists.txt "Example 2"): the CEF tree is a
# sibling directory located via CEF_ROOT, and the application target lives in
# this repository.

cmake_minimum_required(VERSION 3.21)

project(GrokDesktop LANGUAGES CXX RC)

# Keep the generated project focused on the configurations CEF ships binaries
# for. A multi-config generator (Visual Studio) is the supported path on
# Windows; single-config generators need -DCMAKE_BUILD_TYPE=Release.
get_property(_is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(_is_multi_config)
  set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE STRING
      "Supported configurations" FORCE)
else()
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
  endif()
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Use folders in the generated solution for a tidier IDE layout.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#
# Locate the CEF binary distribution.
#
set(CEF_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/third_party/cef")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")

if(NOT EXISTS "${CEF_ROOT}/cmake/FindCEF.cmake")
  message(FATAL_ERROR
          "CEF binary distribution not found at:\n  ${CEF_ROOT}\n"
          "See docs/BUILDING.md for the download and verification steps.")
endif()

# Load the CEF configuration (defines USE_SANDBOX, compiler flags, paths...).
find_package(CEF REQUIRED)

# Include the libcef_dll_wrapper target that ships with the distribution.
add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)

# Include the application target.
add_subdirectory(src)

# Display configuration settings (sandbox mode, flags, file lists).
PRINT_CEF_CONFIG()
