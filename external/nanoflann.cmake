add_library(
  nanoflann
  STATIC
  ${CMAKE_CURRENT_LIST_DIR}/nanoflann/include/nanoflann.hpp)

target_include_directories(nanoflann PUBLIC ${CMAKE_CURRENT_LIST_DIR}/nanoflann/include)
set_target_properties( nanoflann PROPERTIES LINKER_LANGUAGE CXX)
