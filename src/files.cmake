target_include_directories(singing_wind PRIVATE 
  ${CMAKE_CURRENT_LIST_DIR} 
  ${CMAKE_CURRENT_LIST_DIR}/ecs 
  ${CMAKE_CURRENT_LIST_DIR}/render
  ${CMAKE_CURRENT_LIST_DIR}/collision
  ${CMAKE_CURRENT_LIST_DIR}/game
  )

target_sources(singing_wind
  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/main.cpp
  ${CMAKE_CURRENT_LIST_DIR}/input.cpp
  ${CMAKE_CURRENT_LIST_DIR}/game/frame_timer.cpp
  ${CMAKE_CURRENT_LIST_DIR}/game/game.cpp
  ${CMAKE_CURRENT_LIST_DIR}/render/imgui_impl_glfw_gl3.cpp
  ${CMAKE_CURRENT_LIST_DIR}/render/renderer.cpp
  ${CMAKE_CURRENT_LIST_DIR}/render/texture.cpp
  ${CMAKE_CURRENT_LIST_DIR}/render/camera.cpp
  ${CMAKE_CURRENT_LIST_DIR}/render/shader.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ecs/ecs.hpp
  ${CMAKE_CURRENT_LIST_DIR}/collision/col_shape.cpp
  ${CMAKE_CURRENT_LIST_DIR}/collision/collision.cpp
  )
