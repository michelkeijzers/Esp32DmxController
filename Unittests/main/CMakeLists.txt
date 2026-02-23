
cmake_minimum_required(VERSION 3.14)
project(UnitTests)


# Use vcpkg toolchain file if available
if(DEFINED ENV{VCPKG_ROOT})
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
endif()


# Hardcode GTest_DIR for vcpkg integration
set(GTest_DIR "C:/tools/vcpkg/installed/x64-windows/share/gtest" CACHE PATH "Path to GTest cmake config")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_LIST_DIR}/esp_idf_stubs)
include_directories(${CMAKE_SOURCE_DIR}/../../main)

# Add all stub .cpp files from esp_idf_stubs and subfolders
#default_stub_glob_pattern = "${CMAKE_CURRENT_LIST_DIR}/esp_idf_stubs/*.cpp"
file(GLOB STUB_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/esp_idf_stubs/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/esp_idf_stubs/*/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/esp_idf_stubs/*/*/*.cpp"
)

# Use vcpkg's GoogleTest
find_package(GTest CONFIG REQUIRED)


add_executable(
  run_tests
  custom_main.cpp
  dmx_controller_tests.cpp
  ${CMAKE_SOURCE_DIR}/../../main/Tasks/dmx_controller.cpp
  ${CMAKE_SOURCE_DIR}/../../main/Tasks/artnet_sender.cpp
  ${CMAKE_SOURCE_DIR}/../../main/Tasks/osc_sender.cpp
  ${CMAKE_SOURCE_DIR}/../../main/Tasks/seven_segment_display.cpp
  ${CMAKE_SOURCE_DIR}/../../main/Tasks/dmx_preset_changer.cpp
  ${CMAKE_SOURCE_DIR}/../../main/Tasks/foot_switch.cpp
  ${CMAKE_SOURCE_DIR}/../../main/Tasks/nvs_storage.cpp
  ${CMAKE_SOURCE_DIR}/../../main/Tasks/web_server.cpp
  ${CMAKE_SOURCE_DIR}/../../main/Tasks/max3485_sender.cpp
  ${CMAKE_SOURCE_DIR}/../../main/Base/rtos_task.cpp
  ${CMAKE_SOURCE_DIR}/../../main/Data/dmx_presets.cpp
  ${CMAKE_SOURCE_DIR}/../../main/Data/dmx_preset.cpp
  ${STUB_SOURCES}
)

set_target_properties(run_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release
)

target_link_libraries(
  run_tests
  GTest::gtest
  GTest::gmock
)

# Force MSVC linker to include all symbols to ensure test registration


