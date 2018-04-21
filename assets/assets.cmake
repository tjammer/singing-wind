add_custom_target(assets ALL
  DEPENDS ${CMAKE_CURRENT_LIST_DIR}/scripts/map.py
  )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_LIST_DIR}/scripts/map.py
  DEPENDS ${CMAKE_CURRENT_LIST_DIR}/raw/test.blend
  COMMAND blender --background --python ${CMAKE_CURRENT_LIST_DIR}/scripts/map.py
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  )
