add_library(
  json
  STATIC
  ${CMAKE_CURRENT_LIST_DIR}/json/include/nlohmann/json.hpp
  )

target_include_directories(json PUBLIC ${CMAKE_CURRENT_LIST_DIR}/json/include)
set_target_properties(json PROPERTIES LINKER_LANGUAGE CXX)
