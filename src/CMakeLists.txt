cmake_minimum_required (VERSION 2.8)

set(TARGET KulKod CACHE INTERNAL "")

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
  ${CMAKE_CURRENT_LIST_DIR}/launcher.c
)
