cmake_minimum_required(VERSION 3.6)
project(singing_wind)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11 -fno-exceptions -W -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++11 -fno-exceptions -W -Wall -s")

add_subdirectory(external)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${PROJECT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/external/include)
link_directories(${PROJECT_SOURCE_DIR}/external/lib-linux)


find_library(protobuf libprotobuf)
find_package(Protobuf REQUIRED)
PROTOBUF_GENERATE_CPP(protosrc protohdr src/Scene.proto)

set(SOURCE_FILES src/main.cpp
        src/Engine.cpp include/Engine.h
        src/Editor.cpp include/Editor.h
        src/BCurve.cpp include/BCurve.h
        include/WindDefs.h src/Island.cpp
        include/Island.h
        src/EditorStates.cpp
        include/EditorStates.h
        include/triangulate.h
        src/triangulate.cpp
        src/ColShape.cpp
        include/ColShape.h
        src/Collision.cpp
        include/Collision.h
        src/ColGrid.cpp
        include/ColGrid.h src/GameWorld.cpp include/GameWorld.h src/Game.cpp include/Game.h src/Components.cpp include/Components.h include/entities.h src/entities.cpp src/Protagonist.cpp include/Protagonist.h src/systems.cpp include/systems.h src/Camera.cpp include/Camera.h src/FrameTimer.cpp include/FrameTimer.h src/EntityEditor.cpp include/EntityEditor.h src/MoveSystems.cpp include/MoveSystems.h include/imgui-bezier.h src/imgui_impl_glfw_gl3.cpp src/imgui_impl_glfw_gl3.cpp include/stb_image.h src/WRenderer.cpp include/WRenderer.h src/WShader.cpp include/WShader.h src/WTexture.cpp include/WTexture.h src/WInput.cpp include/WInput.h include/WVecMath.h)


#set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
add_executable(singing_wind ${SOURCE_FILES} ${protosrc} ${protohdr})
target_link_libraries(singing_wind imgui glfw glm glad protobuf triangle)

# at compile time
add_custom_command(TARGET singing_wind POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/scenes ${CMAKE_CURRENT_BINARY_DIR}/scenes)

file(GLOB shaders "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.glsl")
message("copy shaders")
foreach(shader ${shaders})
    message("from ${shader}")
    get_filename_component(outputFileName ${shader} NAME)
    message("to ${CMAKE_CURRENT_BINARY_DIR}/shaders/${outputFileName}")
    configure_file(${shader} ${CMAKE_CURRENT_BINARY_DIR}/shaders/${outputFileName} COPYONLY)
endforeach()