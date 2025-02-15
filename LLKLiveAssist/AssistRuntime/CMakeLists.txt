set(TARGET_NAME AssistRuntime)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(USE_RCC TRUE)

# Note: globbing sources is considered bad practice as CMake's generators may not detect new files
# automatically. Keep that in mind when changing files, or explicitly mention them here.
file(GLOB_RECURSE HEADER_FILES "*.h")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")

#source_group(TREE "${ENGINE_ROOT_DIR}/schema" FILES ${SCHEMA_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES})

# 查找qt
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(CMAKE_PREFIX_PATH ${LLKLiveAssist_ROOT_DIR}/Vendor/QT/5.12.12/mingw73_64/lib/cmake/QT5)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(CMAKE_PREFIX_PATH ${LLKLiveAssist_ROOT_DIR}/Vendor/QT/5.12.12/msvc2017_64/lib/cmake/QT5)
endif()
find_package(Qt5 REQUIRED COMPONENTS Core Widgets REQUIRED)





#打包资源素材
if(USE_RCC)
  set(RCC_PROJECT_SOURCES
      ${HEADER_FILES}
      ${SOURCE_FILES}
  )
  qt5_add_big_resources(RCC_PROJECT_SOURCES 
  ${TARGET_NAME}.qrc
  ${LLKLiveAssist_ROOT_DIR}/Vendor/ElaWidgetTools/ElaWidgetTools/include/ElaWidgetTools.qrc)
  add_executable(${TARGET_NAME} ${RCC_PROJECT_SOURCES} )
else()
  add_executable(${TARGET_NAME} ${HEADER_FILES} ${SOURCE_FILES} Resource/LLKLiveAssist.rc)
endif()


set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17 OUTPUT_NAME "AssistRuntime")
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Assist")

target_link_libraries(${TARGET_NAME} LLKAssitCore)

target_include_directories(
  ${TARGET_NAME} 
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)


#copy openssl dll
copy_dll( ${TARGET_NAME} 
          ${LLKLiveAssist_ROOT_DIR}/Vendor/OpenSSL/bin/libssl-3-x64.dll)
copy_dll( ${TARGET_NAME} 
          ${LLKLiveAssist_ROOT_DIR}/Vendor/OpenSSL/bin/libcrypto-3-x64.dll)

if(Qt5Widgets_FOUND)
  target_link_libraries(${TARGET_NAME}  ${Qt5Widgets_LIBRARIES})

  target_include_directories(
    ${TARGET_NAME} 
    PUBLIC ${Qt5Widgets_INCLUDE_DIRS})
endif()

#添加 elawidget
target_link_libraries(${TARGET_NAME}  ElaPacketIO)
target_link_libraries(${TARGET_NAME}  ElaWidgetTools)



if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  copy_dll( ${TARGET_NAME} 
            ${PROJECT_BINARY_DIR}/Vendor/ElaWidgetTools/ElaWidgetTools/ElaWidgetTools.dll)
  copy_dll( ${TARGET_NAME} 
            ${PROJECT_BINARY_DIR}/Vendor/ElaWidgetTools/ElaPacketIO/ElaPacketIO.dll)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  copy_dll( ${TARGET_NAME} 
            ${PROJECT_BINARY_DIR}/Vendor/ElaWidgetTools/ElaWidgetTools/$<CONFIG>/ElaWidgetToolsd.dll)
  copy_dll( ${TARGET_NAME} 
            ${PROJECT_BINARY_DIR}/Vendor/ElaWidgetTools/ElaPacketIO/$<CONFIG>/ElaPacketIOd.dll)
endif()


#Qt 提供了一个名为 windeployqt 的工具，可以自动收集程序依赖的 DLL、插件、翻译文件等，并拷贝到构建目录。

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(QT_PATH ${LLKLiveAssist_ROOT_DIR}/Vendor/QT/5.12.12/mingw73_64)
    set(MY_APP_EXE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}/${TARGET_NAME}/${TARGET_NAME}.exe)   

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${QT_PATH}/bin/windeployqt 
            --debug 
            --no-compiler-runtime 
            --no-translations 
            ${MY_APP_EXE}
            COMMENT "Deploying Qt DLLs with windeployqt...")
    else()
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${QT_PATH}/bin/windeployqt 
            --release  
            --no-compiler-runtime 
            --no-translations 
            ${MY_APP_EXE}
            COMMENT "Deploying Qt DLLs with windeployqt...")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(QT_PATH ${LLKLiveAssist_ROOT_DIR}/Vendor/QT/5.12.12/msvc2017_64)
    set(MY_APP_EXE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/${TARGET_NAME}.exe)   

    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND cmd.exe /c if "$(Configuration)" == "Debug"
    ${QT_PATH}/bin/windeployqt 
    --debug 
    --no-compiler-runtime # 避免拷贝编译器运行时（可选）
    --no-translations     # 可选：不拷贝翻译文件

    ${MY_APP_EXE}         # 指定目标可执行文件
    COMMENT "Deploying Qt DLLs with windeployqt...")   
    
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND cmd.exe /c if "$(Configuration)" == "Release"
    ${QT_PATH}/bin/windeployqt 
    --release  
    --no-compiler-runtime # 避免拷贝编译器运行时（可选）
    --no-translations     # 可选：不拷贝翻译文件
    
    ${MY_APP_EXE}         # 指定目标可执行文件
    COMMENT "Deploying Qt DLLs with windeployqt...")


endif()


install(TARGETS ${TARGET_NAME} LLKAssitCore
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/
)
install(FILES ${LLKLiveAssist_ROOT_DIR}/Config/Model/Model.json DESTINATION ${CMAKE_INSTALL_PREFIX}/Config/Model)

install(IMPORTED_RUNTIME_ARTIFACTS ${TARGET_NAME} 
    DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)