cmake_minimum_required(VERSION 3.4)
project(singing_wind)

set(CMAKE_CXX_STANDARD 17)

# TODO do not use exceptions
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -Wall -DNDEBUG -s")

add_subdirectory(external)
include(src/files.cmake)

option(enable_unity_build "enable unity build for executable" OFF)
if (enable_unity_build)
    include(unity_build.cmake)
    enable_unity_build(singing_wind SOURCE_FILES)
endif(enable_unity_build)

include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR}/include)

if (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread")
    add_executable(singing_wind WIN32 ${SOURCE_FILES} ${protosrc} ${protohdr})
endif()
if (UNIX)
    add_executable(singing_wind ${SOURCE_FILES} ${protosrc} ${protohdr})
endif()
add_dependencies(singing_wind schemas)
target_link_libraries(singing_wind imgui glfw glm glad triangle nanoflann flatbuffers stb_image)

# for ycm
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_custom_command(TARGET singing_wind PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json ${CMAKE_SOURCE_DIR})
