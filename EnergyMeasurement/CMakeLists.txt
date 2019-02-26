cmake_minimum_required (VERSION 2.8)

set(TARGET EnergyMeasurement CACHE INTERNAL "")
set(TARGET_EXE energy_measurement)

add_library(${TARGET} INTERFACE)
add_library(KulKod::${TARGET} ALIAS ${TARGET})

target_include_directories(
  ${TARGET}

  INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}
)

target_sources(
  ${TARGET}

  INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/energy_measurement.c
  ${CMAKE_CURRENT_LIST_DIR}/rapl.c
  ${CMAKE_CURRENT_LIST_DIR}/tplink_plugin.c
)

target_link_libraries(
  ${TARGET}

  INTERFACE
  m
  pthread
)

add_executable(${TARGET_EXE} main.c)
target_link_libraries(
  ${TARGET_EXE}

  ${TARGET}
)

install(TARGETS ${TARGET_EXE} DESTINATION bin)
