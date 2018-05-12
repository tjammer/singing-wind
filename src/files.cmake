target_include_directories(singing_wind PRIVATE 
  ${CMAKE_CURRENT_LIST_DIR} 
  ${CMAKE_CURRENT_LIST_DIR}/util
  ${CMAKE_CURRENT_LIST_DIR}/ecs 
  ${CMAKE_CURRENT_LIST_DIR}/render
  ${CMAKE_CURRENT_LIST_DIR}/collision
  ${CMAKE_CURRENT_LIST_DIR}/game
  ${CMAKE_CURRENT_LIST_DIR}/game/systems
  )

include(${CMAKE_CURRENT_LIST_DIR}/schemas/schemas.cmake)

target_sources(singing_wind
  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/main.cpp
  ${CMAKE_CURRENT_LIST_DIR}/util/input.cpp
  ${CMAKE_CURRENT_LIST_DIR}/util/transform.cpp
  ${CMAKE_CURRENT_LIST_DIR}/game/frame_timer.cpp
  ${CMAKE_CURRENT_LIST_DIR}/game/game.cpp
  ${CMAKE_CURRENT_LIST_DIR}/game/comps.cpp
  ${CMAKE_CURRENT_LIST_DIR}/game/systems/move.cpp
  ${CMAKE_CURRENT_LIST_DIR}/game/systems/input_system.cpp
  ${CMAKE_CURRENT_LIST_DIR}/game/systems/draw.cpp
  ${CMAKE_CURRENT_LIST_DIR}/game/systems/collision_system.cpp
  ${CMAKE_CURRENT_LIST_DIR}/game/systems/fall.cpp
  ${CMAKE_CURRENT_LIST_DIR}/render/imgui_impl_glfw_gl3.cpp
  ${CMAKE_CURRENT_LIST_DIR}/render/renderer.cpp
  ${CMAKE_CURRENT_LIST_DIR}/render/texture.cpp
  ${CMAKE_CURRENT_LIST_DIR}/render/camera.cpp
  ${CMAKE_CURRENT_LIST_DIR}/render/shader.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ecs/ecs.hpp
  ${CMAKE_CURRENT_LIST_DIR}/collision/static_grid.cpp
  ${CMAKE_CURRENT_LIST_DIR}/collision/gjk.cpp
  ${CMAKE_CURRENT_LIST_DIR}/collision/epa.cpp
  ${CMAKE_CURRENT_LIST_DIR}/collision/simplex.cpp
  ${CMAKE_CURRENT_LIST_DIR}/collision/shapes.cpp
  )
