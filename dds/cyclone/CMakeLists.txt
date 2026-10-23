cmake_minimum_required(VERSION 3.20)
project(UMAARealDDSProof LANGUAGES C CXX)
find_package(CycloneDDS REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
include(CTest)
idlc_generate(TARGET demo_battery_types FILES "${CMAKE_CURRENT_SOURCE_DIR}/idl/DemoBattery.idl" WARNINGS no-implicit-extensibility)
add_library(umaa_dds_battery_module MODULE
  "${CMAKE_CURRENT_SOURCE_DIR}/../../sdk/tests/modules/battery.cpp")
target_include_directories(umaa_dds_battery_module PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../sdk/include")
add_executable(umaa_demo_dds_publisher src/publisher.cpp)
target_include_directories(umaa_demo_dds_publisher PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../sdk/include")
target_link_libraries(umaa_demo_dds_publisher PRIVATE demo_battery_types CycloneDDS::ddsc ${CMAKE_DL_LIBS})
add_executable(umaa_demo_dds_simulator ${CMAKE_CURRENT_SOURCE_DIR}/../../simulator/dds/subscriber.c)
target_link_libraries(umaa_demo_dds_simulator PRIVATE demo_battery_types CycloneDDS::ddsc)
add_test(NAME real_dds_battery_report_dispose
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../../simulator/dds/run.py"
          "$<TARGET_FILE:umaa_demo_dds_publisher>" "$<TARGET_FILE:umaa_demo_dds_simulator>"
          "$<TARGET_FILE:umaa_dds_battery_module>")
set_tests_properties(real_dds_battery_report_dispose PROPERTIES TIMEOUT 35)


# Generate official UMAA TypeSupport from AUVSI input only in CI or a user-supplied build tree.
if(DEFINED UMAA_OFFICIAL_GENERATED_ROOT)
  set(OFFICIAL_ROOT "${UMAA_OFFICIAL_GENERATED_ROOT}")
  file(GLOB_RECURSE OFFICIAL_TYPES CONFIGURE_DEPENDS "${OFFICIAL_ROOT}/UMAA/*.c")
  if(NOT OFFICIAL_TYPES)
    message(FATAL_ERROR "Official TypeSupport missing; invoke tools/official_idl_compile.py first")
  endif()
  add_library(umaa_official_type_support STATIC ${OFFICIAL_TYPES})
  target_include_directories(umaa_official_type_support PUBLIC "${OFFICIAL_ROOT}")
  target_link_libraries(umaa_official_type_support PUBLIC CycloneDDS::ddsc)
  add_executable(umaa_official_battery_publisher src/official_battery_publisher.cpp)
  target_include_directories(umaa_official_battery_publisher PRIVATE
      "${CMAKE_CURRENT_SOURCE_DIR}/../../sdk/include")
  target_link_libraries(umaa_official_battery_publisher PRIVATE umaa_official_type_support ${CMAKE_DL_LIBS})
  add_executable(umaa_official_battery_simulator
      "${CMAKE_CURRENT_SOURCE_DIR}/../../simulator/dds/official_battery_subscriber.c")
  target_link_libraries(umaa_official_battery_simulator PRIVATE umaa_official_type_support)
  add_test(NAME official_umaa_battery_dds_wire
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../../simulator/dds/run.py"
            "$<TARGET_FILE:umaa_official_battery_publisher>"
            "$<TARGET_FILE:umaa_official_battery_simulator>"
            "$<TARGET_FILE:umaa_dds_battery_module>")
  set_tests_properties(official_umaa_battery_dds_wire PROPERTIES TIMEOUT 35)
endif()
