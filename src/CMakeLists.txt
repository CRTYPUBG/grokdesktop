# Grok Desktop application target.
# Modeled on the CEF binary distribution's tests/cefsimple/CMakeLists.txt so
# that the sandbox/bootstrap packaging rules stay in sync with the official
# distribution.

#
# Source files.
#

set(GROK_SRCS
  App.cc
  App.h
  BrowserClient.cc
  BrowserClient.h
  MainWindow.cc
  MainWindow.h
  app_config.h
  main.cpp
  )
set(GROK_SRCS_WINDOWS
  ${CMAKE_SOURCE_DIR}/resources/app.rc
  ${CMAKE_SOURCE_DIR}/resources/resource.h
  )
APPEND_PLATFORM_SOURCES(GROK_SRCS)
source_group(grok FILES ${GROK_SRCS} ${GROK_SRCS_WINDOWS})

set(GROK_SRCS
  ${GROK_SRCS}
  ${GROK_SRCS_WINDOWS}
  )

#
# Shared configuration.
#

set(CEF_TARGET "GrokDesktop")

# Logical target used to link the libcef library.
ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")

# Determine the target output directory (build/src/<config>).
SET_CEF_TARGET_OUT_DIR()

#
# Windows configuration.
#

if(USE_SANDBOX)
  # The official Windows sandbox distribution ships bootstrap.exe instead of
  # cef_sandbox.lib, so a sandboxed app is a DLL loaded by bootstrap.exe. The
  # bootstrap binary is copied and renamed to GrokDesktop.exe below.
  add_library(${CEF_TARGET} SHARED ${GROK_SRCS})
  SET_LIBRARY_TARGET_PROPERTIES(${CEF_TARGET})

  # Copy and rename the bootstrap binary so the launcher is GrokDesktop.exe.
  COPY_SINGLE_FILE(${CEF_TARGET} ${CEF_BINARY_DIR}/bootstrap.exe
                   ${CEF_TARGET_OUT_DIR}/${CEF_TARGET}.exe)

  if(CMAKE_GENERATOR MATCHES "Visual Studio")
    # Make the bootstrap binary the default command for the IDE debugger.
    set_target_properties(${CEF_TARGET} PROPERTIES
      VS_DEBUGGER_COMMAND "$<TARGET_FILE_DIR:${CEF_TARGET}>/${CEF_TARGET}.exe"
      )
  endif()
else()
  # Plain Windows executable (no sandbox). The manifest is added afterwards
  # because CEF's shared linker flags use /MANIFEST:NO.
  add_executable(${CEF_TARGET} WIN32 ${GROK_SRCS})
  SET_EXECUTABLE_TARGET_PROPERTIES(${CEF_TARGET})

  ADD_WINDOWS_MANIFEST("${CMAKE_SOURCE_DIR}/resources" "${CEF_TARGET}" "exe")
endif()

add_dependencies(${CEF_TARGET} libcef_dll_wrapper)
target_link_libraries(${CEF_TARGET} libcef_lib libcef_dll_wrapper
                      ${CEF_STANDARD_LIBS} dwmapi)

# Allow "src/..." and "resources/..." style includes from the repository root.
target_include_directories(${CEF_TARGET} PRIVATE ${CMAKE_SOURCE_DIR})

# Copy CEF binary files (libcef.dll, chrome_elf.dll, ...) and resource files
# (Resources/*.pak, icudtl.dat, locales/) next to the target output.
COPY_FILES("${CEF_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}"
           "${CEF_TARGET_OUT_DIR}")
COPY_FILES("${CEF_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}"
           "${CEF_TARGET_OUT_DIR}")

if(USE_SANDBOX)
  # Set LPAC ACLs required for Windows sandbox support.
  SET_LPAC_ACLS("${CEF_TARGET}")
endif()
