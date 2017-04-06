cmake_minimum_required(VERSION 3.6)
project(singing_wind)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -Wall -O0")
set(CMAKE_CXX_FLAGS_RELEASE   "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O2")

add_definitions(-fno-exceptions)

include_directories ("${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/external/include")
link_directories("${PROJECT_SOURCE_DIR}/external/lib")


find_library(protobuf libprotobuf)
find_package(Protobuf REQUIRED)
PROTOBUF_GENERATE_CPP(protosrc protohdr src/map.proto)

set(SOURCE_FILES src/main.cpp src/Engine.cpp include/Engine.h src/Editor.cpp include/Editor.h src/BCurve.cpp include/BCurve.h include/WindDefs.h src/Island.cpp include/Island.h src/WindDefs.cpp src/EditorStates.cpp include/EditorStates.h include/triangulate.h src/triangulate.cpp src/ColShape.cpp include/ColShape.h src/Collision.cpp include/Collision.h src/CollisionTest.cpp include/CollisionTest.h src/ColGrid.cpp include/ColGrid.h)
add_executable(singing_wind ${SOURCE_FILES})
target_link_libraries(singing_wind
        sfml-system
        sfml-window
        sfml-graphics
        triangle
        protobuf)