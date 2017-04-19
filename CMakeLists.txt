cmake_minimum_required(VERSION 3.6)
project(singing_wind)

set(CMAKE_CXX_STANDARD 11)

add_definitions(-fno-exceptions -Wall)

include_directories (${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/external/include
        ${CMAKE_CURRENT_BINARY_DIR})
link_directories("${PROJECT_SOURCE_DIR}/external/lib")


find_library(protobuf libprotobuf)
find_package(Protobuf REQUIRED)
PROTOBUF_GENERATE_CPP(protosrc protohdr src/Scene.proto)

set(SOURCE_FILES src/main.cpp
        src/Engine.cpp include/Engine.h
        src/Editor.cpp include/Editor.h
        src/BCurve.cpp include/BCurve.h
        include/WindDefs.h src/Island.cpp
        include/Island.h src/WindDefs.cpp
        src/EditorStates.cpp
        include/EditorStates.h
        include/triangulate.h
        src/triangulate.cpp
        src/ColShape.cpp
        include/ColShape.h
        src/Collision.cpp
        include/Collision.h
        src/ColGrid.cpp
        include/ColGrid.h
        external/src/imgui.cpp
        external/src/imgui_draw.cpp
        external/src/imgui-SFML.cpp src/GameWorld.cpp include/GameWorld.h src/Game.cpp include/Game.h src/Components.cpp include/Components.h include/entities.h src/entities.cpp src/Protagonist.cpp include/Protagonist.h src/systems.cpp include/systems.h src/Camera.cpp include/Camera.h)
add_executable(singing_wind ${SOURCE_FILES} ${protosrc} ${protohdr})
target_link_libraries(singing_wind
        sfml-system
        sfml-window
        sfml-graphics
        triangle
        protobuf
        GL)