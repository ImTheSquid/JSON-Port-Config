cmake_minimum_required(VERSION 3.0.0)

if(NOT DEFINED ENV{VCPKG_ROOT})
  message(FATAL_ERROR "VCPKG required")
endif()

if(WIN32)
  message("Detected Win32")
  add_compile_options(/std:c++latest)
elseif(UNIX)
  message("Detected Linux")
  set(VCPKG_TARGET_TRIPLET x64-linux CACHE STRING "")
endif()

set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
if(NOT EXISTS ${CMAKE_TOOLCHAIN_FILE})
  message(FATAL_ERROR "No toolchain file at ${CMAKE_TOOLCHAIN_FILE}")
endif()
include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

project(JSON-Port-Config VERSION 1.6)

include(CTest)
enable_testing()

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(Libssh2 REQUIRED)

add_executable(JSON-Port-Config main.cpp GUI.cpp TypeLoader.cpp Uploader.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

target_link_libraries(JSON-Port-Config PRIVATE Qt5::Core Qt5::Widgets Libssh2::libssh2)