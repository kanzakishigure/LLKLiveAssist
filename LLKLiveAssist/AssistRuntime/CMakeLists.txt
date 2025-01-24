set(TARGET_NAME AssistRuntime)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Note: globbing sources is considered bad practice as CMake's generators may not detect new files
# automatically. Keep that in mind when changing files, or explicitly mention them here.
file(GLOB_RECURSE HEADER_FILES "*.h")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")

#source_group(TREE "${ENGINE_ROOT_DIR}/schema" FILES ${SCHEMA_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES})

add_executable(${TARGET_NAME} ${HEADER_FILES} ${SOURCE_FILES})


set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17 OUTPUT_NAME "AssistRuntime")
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Assist")

target_link_libraries(${TARGET_NAME} LLKAssitCore)



if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${LLKLiveAssist_ROOT_DIR}/Vendor/OpenSSL/bin/libssl-3-x64.dll
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}/${TARGET_NAME}/$<CONFIG>/)

    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${LLKLiveAssist_ROOT_DIR}/Vendor/OpenSSL/bin/libcrypto-3-x64.dll
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}/${TARGET_NAME}/$<CONFIG>/)
    message("COPY openssl DLL ${LLKLiveAssist_ROOT_DIR}/Vendor/OpenSSL/bin/libcrypto-3-x64.dll")
    message("COPY openssl DLL ${LLKLiveAssist_ROOT_DIR}/Vendor/OpenSSL/bin/libssl-3-x64.dll")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
     add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${LLKLiveAssist_ROOT_DIR}/Vendor/OpenSSL/bin/libssl-3-x64.dll
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}/${TARGET_NAME}/)

    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${LLKLiveAssist_ROOT_DIR}/Vendor/OpenSSL/bin/libcrypto-3-x64.dll
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}/${TARGET_NAME}/)
    message("COPY openssl DLL ${LLKLiveAssist_ROOT_DIR}/Vendor/OpenSSL/bin/libcrypto-3-x64.dll")
    message("COPY openssl DLL ${LLKLiveAssist_ROOT_DIR}/Vendor/OpenSSL/bin/libssl-3-x64.dll")

endif()