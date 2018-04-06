target_include_directories(singing_wind PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/ecs)

target_sources(singing_wind
  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/main.cpp
  ${CMAKE_CURRENT_LIST_DIR}/input.cpp
  ${CMAKE_CURRENT_LIST_DIR}/shader.cpp
  ${CMAKE_CURRENT_LIST_DIR}/col_shape.cpp
  ${CMAKE_CURRENT_LIST_DIR}/texture.cpp
  ${CMAKE_CURRENT_LIST_DIR}/collision.cpp
  ${CMAKE_CURRENT_LIST_DIR}/renderer.cpp
  ${CMAKE_CURRENT_LIST_DIR}/frame_timer.cpp
  ${CMAKE_CURRENT_LIST_DIR}/imgui_impl_glfw_gl3.cpp
  ${CMAKE_CURRENT_LIST_DIR}/camera.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ecs/ecs.hpp
  )
