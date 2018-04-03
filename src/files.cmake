target_include_directories(singing_wind PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_sources(singing_wind
  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/main.cpp
  ${CMAKE_CURRENT_LIST_DIR}/WInput.cpp
  ${CMAKE_CURRENT_LIST_DIR}/WShader.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ColShape.cpp
  ${CMAKE_CURRENT_LIST_DIR}/WTexture.cpp
  ${CMAKE_CURRENT_LIST_DIR}/Collision.cpp
  ${CMAKE_CURRENT_LIST_DIR}/WRenderer.cpp
  ${CMAKE_CURRENT_LIST_DIR}/FrameTimer.cpp
  ${CMAKE_CURRENT_LIST_DIR}/imgui_impl_glfw_gl3.cpp
  )
