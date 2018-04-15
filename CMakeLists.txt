cmake_minimum_required(VERSION 3.4)
project(singing_wind)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -fno-exceptions -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -fno-exceptions -Wall -DNDEBUG -s")

add_subdirectory(external)
include_directories(${PROJECT_BINARY_DIR})

add_executable(singing_wind "")
include(src/files.cmake)

if (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread")
    add_executable(singing_wind WIN32 ${SOURCE_FILES} ${protosrc} ${protohdr})
endif()
#add_dependencies(singing_wind schemas)
target_link_libraries(singing_wind imgui glfw glm glad stb_image json)

# for ycm
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
