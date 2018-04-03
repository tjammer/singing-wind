add_library(
  stb_image
  STATIC
  ${CMAKE_CURRENT_LIST_DIR}/stb/stb_image.h
  )
target_include_directories(stb_image PUBLIC ${CMAKE_CURRENT_LIST_DIR}/stb)
set_target_properties(stb_image PROPERTIES LINKER_LANGUAGE CXX)
