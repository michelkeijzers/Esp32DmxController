cmake_minimum_required(VERSION 3.14)
project(UnitTests)

set(CMAKE_CXX_STANDARD 17)

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
  simple_test.cpp
  dmx_controller_tests.cpp
  ${CMAKE_SOURCE_DIR}/../../main/dmx_controller.cpp
  ${CMAKE_SOURCE_DIR}/../../main/artnet_sender.cpp
  ${CMAKE_SOURCE_DIR}/../../main/osc_sender.cpp
  ${CMAKE_SOURCE_DIR}/../../main/seven_segment_display.cpp
  ${CMAKE_SOURCE_DIR}/../../main/dmx_preset_changer.cpp
  ${CMAKE_SOURCE_DIR}/../../main/foot_switch.cpp
  ${CMAKE_SOURCE_DIR}/../../main/nvs_storage.cpp
  ${CMAKE_SOURCE_DIR}/../../main/web_server.cpp
  ${CMAKE_SOURCE_DIR}/../../main/rtos_task.cpp
  ${CMAKE_SOURCE_DIR}/../../main/dmx_presets.cpp
  ${CMAKE_SOURCE_DIR}/../../main/dmx_preset.cpp
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
if(MSVC)
  target_link_options(run_tests PRIVATE /OPT:NOREF)
endif()

