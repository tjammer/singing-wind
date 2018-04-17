set (flatbuffers_schemas
    ${CMAKE_CURRENT_LIST_DIR}/map.fbs
    )

set(FLATBUFFERS_FLATC_SCHEMA_EXTRA_ARGS "--cpp" "--python")

build_flatbuffers("${flatbuffers_schemas}" "${PROJECT_SOURCE_DIR}" schemas "" "${PROJECT_BINARY_DIR}" "" "")
