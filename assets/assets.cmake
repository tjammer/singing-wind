add_custom_target(assets ALL
  DEPENDS ${PROJECT_BINARY_DIR}/test.swmap
  )

add_custom_command(
  OUTPUT ${PROJECT_BINARY_DIR}/test.swmap
  DEPENDS ${CMAKE_CURRENT_LIST_DIR}/raw/test.blend
  COMMAND blender --background --python ${CMAKE_CURRENT_LIST_DIR}/scripts/map.py
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  )
