# Optional: Set path to OpenCppCoverage.exe if not in PATH
set(OPENCOVERAGE_EXE "C:/OpenCppCoverage/OpenCppCoverage.exe" CACHE STRING "Path to OpenCppCoverage.exe")

# Custom target to run coverage for dmx_controller.cpp
add_custom_target(coverage
  COMMAND ${OPENCOVERAGE_EXE}
    --export_type=html:coverage_report
    --sources "C:\\Users\\i4studio\\Esp32Test\\Esp32DmxController\\main\\Tasks\\dmx_controller.cpp"
    -- "${CMAKE_CURRENT_BINARY_DIR}/build/Debug/run_tests.exe"
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Running OpenCppCoverage for dmx_controller.cpp"
  VERBATIM
)

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
  web_server_api_tests.cpp
  ${CMAKE_SOURCE_DIR}/../../main/Tasks/dmx_controller.cpp
  ${CMAKE_SOURCE_DIR}/../../main/Tasks/seven_segment_display.cpp
  ${CMAKE_SOURCE_DIR}/../../main/Tasks/foot_switch.cpp
  ${CMAKE_SOURCE_DIR}/../../main/Tasks/nv_storage.cpp
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

if(WIN32)
  add_custom_command(TARGET run_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:run_tests> $<TARGET_RUNTIME_DLLS:run_tests>
    COMMAND_EXPAND_LISTS
  )
endif()


