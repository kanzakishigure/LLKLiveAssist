set(PLUGIN_ROOT_DIR "${NAZAKI_ROOT_DIR}/Plugin")

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options("/MP")
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT AssistRuntime)
endif()

add_subdirectory(LLKAssitCore)
add_subdirectory(AssistRuntime)