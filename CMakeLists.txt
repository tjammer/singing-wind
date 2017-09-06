cmake_minimum_required(VERSION 3.6)
project(singing_wind)

set(CMAKE_CXX_STANDARD 14)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -W -Wall")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}  -fno-exceptions -W -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -fno-exceptions -W -Wall -DNDEBUG -s")

add_subdirectory(external)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/skills
    ${PROJECT_BINARY_DIR})

set(INCLUDE_FILES include/Engine.h
    include/Editor.h
    include/BCurve.h
    include/WindDefs.h
    include/WVecMath.h
    include/Island.h
    include/EditorStates.h
    include/triangulate.h
    include/ColShape.h
    include/Collision.h
    include/ColGrid.h
    include/GameWorld.h
    include/Game.h
    include/Components.h
    include/entities.h
    include/Protagonist.h
    include/systems.h
    include/Camera.h
    include/FrameTimer.h
    include/EntityEditor.h
    include/MoveSystems.h
    include/imgui-bezier.h
    include/stb_image.h
    include/WRenderer.h
    include/WShader.h
    include/WTexture.h
    include/WInput.h
    include/SceneIO.h
    include/NavMesh.h
    include/SimpleFlyer.h
    include/InputComponent.h
    include/CollisionComponent.h
    include/PosComponent.h
    include/Pathfinding.h
    include/SkillComponent.h
    include/TagComponent.h
    include/CPruneSweep.h
    include/Actor.h
    skills/Melee.h
    include/HurtBoxComponent.h
    include/LifeTimeComponent.h
    include/StatusEffectComponent.h
    include/AIComponent.h
    include/ai_state_transitions.h
    include/ai_state_funcs.h
    include/ai_input_funcs.h
    include/PatrolComponent.h
    skills/Lounge.h)

set(SOURCE_FILES src/main.cpp
    src/Engine.cpp
    src/Editor.cpp
    src/BCurve.cpp
    src/Island.cpp
    src/EditorStates.cpp
    src/triangulate.cpp
    src/ColShape.cpp
    src/Collision.cpp
    src/ColGrid.cpp
    src/GameWorld.cpp
    src/Game.cpp
    src/Components.cpp
    src/entities.cpp
    src/Protagonist.cpp
    src/systems.cpp
    src/Camera.cpp
    src/FrameTimer.cpp
    src/EntityEditor.cpp
    src/MoveSystems.cpp
    src/imgui_impl_glfw_gl3.cpp
    src/WRenderer.cpp
    src/WShader.cpp
    src/WTexture.cpp
    src/WInput.cpp
    src/SceneIO.cpp
    src/NavMesh.cpp
    src/SimpleFlyer.cpp
    src/InputComponent.cpp
    src/CollisionComponent.cpp
    src/Pathfinding.cpp
    src/SkillComponent.cpp
    src/CPruneSweep.cpp
    src/TagComponent.cpp
    src/Actor.cpp
    skills/Melee.cpp
    src/HurtBoxComponent.cpp
    src/StatusEffectComponent.cpp
    src/AIComponent.cpp
    src/ai_state_transitions.cpp
    src/ai_state_funcs.cpp
    src/ai_input_funcs.cpp
    src/PatrolComponent.cpp
    skills/Lounge.cpp
    src/PosComponent.cpp)

set (flatbuffers_schemas
    ${PROJECT_SOURCE_DIR}/schemas/entity.fbs
    ${PROJECT_SOURCE_DIR}/schemas/scene.fbs)

set(FLATBUFFERS_FLATC_SCHEMA_EXTRA_ARGS "--gen-object-api")

build_flatbuffers("${flatbuffers_schemas}" "${PROJECT_SOURCE_DIR}" schemas "" "${PROJECT_BINARY_DIR}" "" "")

option(enable_unity_build "enable unity build for executable" OFF)
if (enable_unity_build)
    include(unity_build.cmake)
    enable_unity_build(singing_wind SOURCE_FILES)
endif(enable_unity_build)

add_executable(singing_wind ${SOURCE_FILES} ${protosrc} ${protohdr})
add_dependencies(singing_wind schemas)
target_link_libraries(singing_wind imgui glfw glm glad triangle nanoflann flatbuffers)

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

# for ycm
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_custom_command(TARGET singing_wind PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json ${CMAKE_SOURCE_DIR})
