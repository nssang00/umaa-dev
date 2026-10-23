cmake_minimum_required(VERSION 3.20)
project(UMAARealDDSProof LANGUAGES C)
find_package(CycloneDDS REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
include(CTest)
idlc_generate(TARGET demo_battery_types FILES "${CMAKE_CURRENT_SOURCE_DIR}/idl/DemoBattery.idl" WARNINGS no-implicit-extensibility)
add_executable(umaa_demo_dds_publisher src/publisher.c)
target_link_libraries(umaa_demo_dds_publisher PRIVATE demo_battery_types CycloneDDS::ddsc)
add_executable(umaa_demo_dds_simulator ${CMAKE_CURRENT_SOURCE_DIR}/../../simulator/dds/subscriber.c)
target_link_libraries(umaa_demo_dds_simulator PRIVATE demo_battery_types CycloneDDS::ddsc)
add_test(NAME real_dds_battery_report_dispose
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../../simulator/dds/run.py"
          "$<TARGET_FILE:umaa_demo_dds_publisher>" "$<TARGET_FILE:umaa_demo_dds_simulator>")
set_tests_properties(real_dds_battery_report_dispose PROPERTIES TIMEOUT 35)
