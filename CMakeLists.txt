cmake_minimum_required(VERSION 3.4)
project(singing_wind)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -fno-exceptions -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -fno-exceptions -Wall -DNDEBUG -s")

add_subdirectory(external)
include_directories(${PROJECT_BINARY_DIR})

if (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread")
endif()

add_executable(singing_wind "")
include(src/files.cmake)

include(assets/assets.cmake)
add_dependencies(assets schemas)
add_dependencies(singing_wind schemas assets)
target_link_libraries(singing_wind imgui glfw glm glad stb_image flatbuffers)

# for ycm
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_custom_command(TARGET singing_wind PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json ${CMAKE_SOURCE_DIR})
