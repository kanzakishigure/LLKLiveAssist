cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(LLKLiveAssist VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ---- Include guards ----
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
set(LLKLiveAssist_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

#dll copy function 
function(copy_dll target_name source)
  if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_custom_command(TARGET ${target_name} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${source}
    ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/)
    message("COPY DLL " ${source} " TO "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/)
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_custom_command(TARGET ${target_name} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${source}
    ${CMAKE_CURRENT_BINARY_DIR})
    message("COPY DLL " ${source} " TO " ${CMAKE_CURRENT_BINARY_DIR})
  endif()
  
endfunction()


add_subdirectory(Vendor/ElaWidgetTools)
add_subdirectory(LLKLiveAssist)


