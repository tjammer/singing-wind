cmake_minimum_required(VERSION 3.6)
project(singing_wind)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11 -fno-exceptions -W -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++11 -fno-exceptions -W -Wall -s")

add_definitions(-DSFML_STATIC)

include_directories (${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/external/include
        ${CMAKE_CURRENT_BINARY_DIR})
link_directories("${PROJECT_SOURCE_DIR}/external/lib")
if (UNIX)
    link_directories("${PROJECT_SOURCE_DIR}/external/lib-linux")
endif()
if (WIN32)
    link_directories("${PROJECT_SOURCE_DIR}/external/lib-windows")
endif()


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
        include/ColGrid.h
        external/src/imgui.cpp
        external/src/imgui_draw.cpp
        external/src/imgui-SFML.cpp src/GameWorld.cpp include/GameWorld.h src/Game.cpp include/Game.h src/Components.cpp include/Components.h include/entities.h src/entities.cpp src/Protagonist.cpp include/Protagonist.h src/systems.cpp include/systems.h src/Camera.cpp include/Camera.h src/FrameTimer.cpp include/FrameTimer.h src/EntityEditor.cpp include/EntityEditor.h src/MoveSystems.cpp include/MoveSystems.h include/imgui-bezier.h)

if (UNIX)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
    add_executable(singing_wind ${SOURCE_FILES} ${protosrc} ${protohdr})
    target_link_libraries(singing_wind sfml-graphics-s sfml-window-s sfml-system-s triangle GL protobuf jpeg udev Xrandr X11 pthread)
endif()
if (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread")
    add_executable(singing_wind WIN32 ${SOURCE_FILES} ${protosrc} ${protohdr})
    target_link_libraries(singing_wind sfml-graphics-s sfml-window-s sfml-system-s triangle protobuf freetype jpeg opengl32 gdi32 winmm)
endif()

# at compile time
add_custom_command(TARGET singing_wind POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/scenes ${CMAKE_CURRENT_BINARY_DIR}/scenes)