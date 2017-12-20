include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/engine
    ${CMAKE_CURRENT_LIST_DIR}/skills
    ${CMAKE_CURRENT_LIST_DIR}/behaviours
    )

include(${CMAKE_CURRENT_LIST_DIR}/engine/engine.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/skills/skills.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/behaviours/behaviours.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/schemas/schemas.cmake)

set(SOURCE_FILES
    ${ENGINE_SOURCE}
    ${SKILLS_SOURCE}
    ${BEHAVIOURS_SOURCE}
    )
