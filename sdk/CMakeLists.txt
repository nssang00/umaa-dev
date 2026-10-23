cmake_minimum_required(VERSION 3.20)
project(UMAAPrototype VERSION 0.1.0 LANGUAGES CXX)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
add_library(umaa_sdk_core INTERFACE)
add_library(UMAA::Core ALIAS umaa_sdk_core)
target_compile_features(umaa_sdk_core INTERFACE cxx_std_17)
target_include_directories(umaa_sdk_core INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
set_target_properties(umaa_sdk_core PROPERTIES EXPORT_NAME Core)
option(UMAA_BUILD_TESTS "Build standalone core tests" ON)
if(UMAA_BUILD_TESTS)
  enable_testing()
  add_executable(umaa_command_engine_test tests/test_engine.cpp)
  target_link_libraries(umaa_command_engine_test PRIVATE UMAA::Core)
  add_test(NAME umaa_command_engine_test COMMAND umaa_command_engine_test)
  add_executable(umaa_report_collection_test tests/test_report_collection.cpp)
  target_link_libraries(umaa_report_collection_test PRIVATE UMAA::Core)
  add_test(NAME umaa_report_collection_test COMMAND umaa_report_collection_test)
endif()
install(TARGETS umaa_sdk_core EXPORT UMAASDKTargets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT UMAASDKTargets FILE UMAASDKTargets.cmake NAMESPACE UMAA::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/UMAAPrototype)
configure_package_config_file(cmake/UMAAPrototypeConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/UMAAPrototypeConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/UMAAPrototype)
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/UMAAPrototypeConfigVersion.cmake"
  COMPATIBILITY SameMajorVersion)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/UMAAPrototypeConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/UMAAPrototypeConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/UMAAPrototype)

# Prototype native service modules: SDK keeps UMAA wire behavior outside modules.
option(UMAA_BUILD_MODULE_DEMO "Build cross-platform dynamic service module contract tests" ON)
if(UMAA_BUILD_TESTS AND UMAA_BUILD_MODULE_DEMO)
  add_library(umaa_demo_engine MODULE tests/modules/engine.cpp)
  add_library(umaa_demo_battery MODULE tests/modules/battery.cpp)
  add_library(umaa_demo_wrong_abi MODULE tests/modules/wrong_abi.cpp)
  target_link_libraries(umaa_demo_engine PRIVATE UMAA::Core)
  target_link_libraries(umaa_demo_battery PRIVATE UMAA::Core)
  target_link_libraries(umaa_demo_wrong_abi PRIVATE UMAA::Core)
  add_executable(umaa_dynamic_module_test tests/test_dynamic_module.cpp)
  target_link_libraries(umaa_dynamic_module_test PRIVATE UMAA::Core ${CMAKE_DL_LIBS})
  add_test(NAME umaa_dynamic_module_test COMMAND umaa_dynamic_module_test
    "$<TARGET_FILE:umaa_demo_engine>" "$<TARGET_FILE:umaa_demo_battery>"
    "$<TARGET_FILE:umaa_demo_wrong_abi>")
endif()

# Fixture-only generated code; separate from official UMAA TypeSupport.
if(UMAA_BUILD_TESTS)
  find_package(Python3 COMPONENTS Interpreter REQUIRED)
  set(UMAA_TEST_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
  add_custom_command(
    OUTPUT "${UMAA_TEST_GENERATED_DIR}/include/umaa/generated/EngineControl.hpp"
           "${UMAA_TEST_GENERATED_DIR}/include/umaa/generated/BatteryStatus.hpp"
           "${UMAA_TEST_GENERATED_DIR}/include/umaa/generated/types.hpp"
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/umaa_codegen.py"
       generate --idl "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/demo.idl"
       --catalog "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/services.json"
       --service Demo::EO::EngineControl --out "${UMAA_TEST_GENERATED_DIR}"
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/umaa_codegen.py"
       generate --idl "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/demo.idl"
       --catalog "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/services.json"
       --service Demo::EO::BatteryStatus --out "${UMAA_TEST_GENERATED_DIR}"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tools/umaa_codegen.py"
            "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/demo.idl"
            "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/services.json"
    VERBATIM)
  add_custom_target(umaa_fixture_codegen DEPENDS
     "${UMAA_TEST_GENERATED_DIR}/include/umaa/generated/EngineControl.hpp"
     "${UMAA_TEST_GENERATED_DIR}/include/umaa/generated/BatteryStatus.hpp")
  add_executable(umaa_mock_engine_host examples/mock_engine_host.cpp)
  add_executable(umaa_mock_report_host examples/mock_report_host.cpp)
  foreach(t umaa_mock_engine_host umaa_mock_report_host)
    add_dependencies(${t} umaa_fixture_codegen)
    target_link_libraries(${t} PRIVATE UMAA::Core)
    target_include_directories(${t} PRIVATE "${UMAA_TEST_GENERATED_DIR}/include")
  endforeach()
  add_test(NAME umaa_generated_report_smoke COMMAND umaa_mock_report_host)
endif()
