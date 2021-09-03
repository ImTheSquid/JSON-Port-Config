cmake_minimum_required(VERSION 3.0.0)

if(DEFINED ENV{VCPKG_ROOT})
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
  include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
    message("FOUND TOOLCHAIN FILE")
else()
    message(FATAL_ERROR "NO TOOLCHAIN FILE FOUND")
endif()

if(WIN32)
  message("Detected Win32")
  add_compile_options(/std:c++latest)
endif()

project(JSON-Port-Config VERSION 0.1.0)

include(CTest)
enable_testing()

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(Libssh2 REQUIRED)

add_executable(JSON-Port-Config main.cpp GUI.cpp TypeLoader.cpp Uploader.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

target_link_libraries(JSON-Port-Config PRIVATE Qt5::Core Qt5::Widgets Libssh2::libssh2)