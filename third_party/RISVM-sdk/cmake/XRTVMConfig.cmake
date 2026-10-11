# XRTVM SDK CMake Integration
# Usage: find_package(XRTVM REQUIRED)

cmake_minimum_required(VERSION 3.16)

set(XRTVM_VERSION "1.0.0")
set(XRTVM_FOUND TRUE)

# Compiler SDK
set(XRTVM_COMPILER_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/../compiler/include")
set(XRTVM_COMPILER_LIB "${CMAKE_CURRENT_LIST_DIR}/../compiler/lib/xrtvm_compiler.lib")

# Runtime SDK
set(XRTVM_RUNTIME_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/../runtime/include")
set(XRTVM_RUNTIME_LIB "${CMAKE_CURRENT_LIST_DIR}/../runtime/lib/xrtvm_runtime.lib")

# Combined include dirs
set(XRTVM_INCLUDE_DIRS
    ${XRTVM_COMPILER_INCLUDE_DIR}
    ${XRTVM_RUNTIME_INCLUDE_DIR}
)

# Import targets
if(NOT TARGET XRTVM::Compiler)
    add_library(XRTVM::Compiler STATIC IMPORTED)
    set_target_properties(XRTVM::Compiler PROPERTIES
        IMPORTED_LOCATION "${XRTVM_COMPILER_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${XRTVM_COMPILER_INCLUDE_DIR}"
    )
endif()

if(NOT TARGET XRTVM::Runtime)
    add_library(XRTVM::Runtime STATIC IMPORTED)
    set_target_properties(XRTVM::Runtime PROPERTIES
        IMPORTED_LOCATION "${XRTVM_RUNTIME_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${XRTVM_RUNTIME_INCLUDE_DIR}"
    )
endif()

message(STATUS "Found XRTVM SDK version ${XRTVM_VERSION}")
