include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/skills)

set(GAMEPLAY_SOURCE
    ${CMAKE_CURRENT_LIST_DIR}/skills/Melee.cpp
    ${CMAKE_CURRENT_LIST_DIR}/skills/Lounge.cpp
    )

set (flatbuffers_schemas
    ${CMAKE_CURRENT_LIST_DIR}/schemas/entity.fbs
    ${CMAKE_CURRENT_LIST_DIR}/schemas/scene.fbs)

set(FLATBUFFERS_FLATC_SCHEMA_EXTRA_ARGS "--gen-object-api")

build_flatbuffers("${flatbuffers_schemas}" "${PROJECT_SOURCE_DIR}" schemas "" "${PROJECT_BINARY_DIR}" "" "")

