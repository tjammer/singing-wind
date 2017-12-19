cmake_minimum_required(VERSION 3.4)
project(singing_wind)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}  -fno-exceptions -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -fno-exceptions -Wall -DNDEBUG -s")

add_subdirectory(external)
include(engine/engine.cmake)
include(gameplay/gameplay.cmake)
set(SOURCE_FILES
    ${ENGINE_SOURCE}
    ${GAMEPLAY_SOURCE})

option(enable_unity_build "enable unity build for executable" OFF)
if (enable_unity_build)
    include(unity_build.cmake)
    enable_unity_build(singing_wind SOURCE_FILES)
endif(enable_unity_build)

include_directories(${PROJECT_BINARY_DIR})

if (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread")
    add_executable(singing_wind WIN32 ${SOURCE_FILES} ${protosrc} ${protohdr})
endif()
if (UNIX)
    add_executable(singing_wind ${SOURCE_FILES} ${protosrc} ${protohdr})
endif()
add_dependencies(singing_wind schemas)
target_link_libraries(singing_wind imgui glfw glm glad triangle nanoflann flatbuffers)

# at compile time
add_custom_command(TARGET singing_wind POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/scenes ${CMAKE_CURRENT_BINARY_DIR}/scenes)

file(GLOB shaders "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.glsl")
# message("copy shaders")
foreach(shader ${shaders})
    # message("from ${shader}")
    get_filename_component(outputFileName ${shader} NAME)
    # message("to ${CMAKE_CURRENT_BINARY_DIR}/shaders/${outputFileName}")
    configure_file(${shader} ${CMAKE_CURRENT_BINARY_DIR}/shaders/${outputFileName} COPYONLY)
endforeach()

# for ycm
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_custom_command(TARGET singing_wind PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json ${CMAKE_SOURCE_DIR})
