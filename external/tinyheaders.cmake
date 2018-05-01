add_library(
  tinyheaders
  STATIC
  ${CMAKE_CURRENT_LIST_DIR}/tinyheaders/tinymath2d.h
  ${CMAKE_CURRENT_LIST_DIR}/tinyheaders/tinymath.h
  ${CMAKE_CURRENT_LIST_DIR}/tinyheaders/tinygl.h
  )
target_compile_options(tinyheaders INTERFACE -w)
target_include_directories(tinyheaders PUBLIC ${CMAKE_CURRENT_LIST_DIR}/tinyheaders)
set_target_properties(tinyheaders PROPERTIES LINKER_LANGUAGE CXX)
