set(TARGET_NAME LLKAssitCore)
#search recurse all files
file(GLOB_RECURSE ASSISTPLUGIN_HEADERS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB_RECURSE ASSISTPLUGIN_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)


source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${ASSISTPLUGIN_HEADERS} ${ASSISTPLUGIN_SOURCES} )

add_library(${TARGET_NAME}  ${ASSISTPLUGIN_HEADERS} ${ASSISTPLUGIN_SOURCES})

target_include_directories(
  ${TARGET_NAME} 
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Assist")


target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->") #将警告视为错误

# 添加项目预定义宏文件



if(CMAKE_BUILD_TYPE STREQUAL "Debug")

  target_compile_definitions(${TARGET_NAME} PRIVATE GPT_SOVITS_ROOT="${LLKLiveAssist_ROOT_DIR}/GSoVITSCore/GPT-SoVITS-v2/")
  target_compile_definitions(${TARGET_NAME} PRIVATE MODEL_DATA_PATH="${LLKLiveAssist_ROOT_DIR}/Config/Model/Model.json")
  target_compile_definitions(${TARGET_NAME} PRIVATE MODEL_CONFIG_PATH="${LLKLiveAssist_ROOT_DIR}/Config/Model/ModelSetting.json")
  target_compile_definitions(${TARGET_NAME} PRIVATE BILI_CONFIG_PATH="${LLKLiveAssist_ROOT_DIR}/Config/BiliClientAssistSetting.json")

  target_compile_definitions(${TARGET_NAME} PRIVATE INFER_DATA_PATH="${LLKLiveAssist_ROOT_DIR}/Config/Model/InferSetting.json")

  target_compile_definitions(${TARGET_NAME} PRIVATE LLKDebug)
  target_compile_definitions(${TARGET_NAME} PUBLIC LLK_WORK_SPACE="${LLKLiveAssist_ROOT_DIR}")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")

  target_compile_definitions(${TARGET_NAME} PRIVATE GPT_SOVITS_ROOT="GSoVITSCore/GPT-SoVITS-v2/")
  target_compile_definitions(${TARGET_NAME} PRIVATE MODEL_DATA_PATH="Config/Model/Model.json")
  target_compile_definitions(${TARGET_NAME} PRIVATE MODEL_CONFIG_PATH="Config/Model/ModelSetting.json")
  target_compile_definitions(${TARGET_NAME} PRIVATE BILI_CONFIG_PATH="Config/BiliClientAssistSetting.json")
  target_compile_definitions(${TARGET_NAME} PRIVATE INFER_DATA_PATH="Config/Model/InferSetting.json")

  target_compile_definitions(${TARGET_NAME} PRIVATE LLKRelease)
endif()

#查找spd库
if(NOT TARGET spdlog)
    # Stand-alone build
    set(spdlog_ROOT "${LLKLiveAssist_ROOT_DIR}/Vendor/spdlog")
    find_package(spdlog REQUIRED)
    if(spdlog_FOUND)
      message(STATUS "spdlog_INCLUDE_DIRS : ${spdlog_INCLUDE_DIRS}")
      message(STATUS "spdlog_LIBRARY_DIRS : ${spdlog_LIBRARY_DIRS}")
      message(STATUS "spdlog_LIBRARIES :    ${spdlog_LIBRARIES}")
      
      target_link_libraries(${TARGET_NAME}  spdlog::spdlog )
    endif()
    
endif()

# 查找 Boost 库

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(BOOST_ROOT "${LLKLiveAssist_ROOT_DIR}/Vendor/boost/BoostBuild-GCC")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(BOOST_ROOT "${LLKLiveAssist_ROOT_DIR}/Vendor/boost/BoostBuild-MSVC")
endif()

set(Boost_INCLUDE_DIRS "${BOOST_ROOT}/include")
set(Boost_LIBRARY_DIRS "${BOOST_ROOT}/lib")

find_package(Boost 1.87 REQUIRED COMPONENTS system regex json process)
if(Boost_FOUND)
  link_directories(${Boost_LIBRARY_DIRS})
  include_directories(${Boost_INCLUDE_DIRS})
  target_link_libraries(${TARGET_NAME} ${Boost_LIBRARIES})
  
  target_include_directories(
  ${TARGET_NAME} 
  PUBLIC ${Boost_INCLUDE_DIRS})

  message(STATUS "BOOST_ROOT: ${BOOST_ROOT}")
  message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
  message(STATUS "Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
  message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
endif()

#find ssl

find_package(OpenSSL REQUIRED)
set(OPENSSL_ROOT_DIR "${LLKLiveAssist_ROOT_DIR}/Vendor/OpenSSL")
set(OPENSSL_LINK_DIR "${LLKLiveAssist_ROOT_DIR}/Vendor/OpenSSL/lib")

set(OPENSSL_USE_STATIC_LIBS TRUE)

if(OPENSSL_FOUND)
  
  include_directories(${OPENSSL_INCLUDE_DIR})
  link_directories(${OPENSSL_ROOT_DIR}/bin)
  link_directories(${OPENSSL_LINK_DIR})

  target_link_libraries(${TARGET_NAME}  OpenSSL::SSL)
  target_link_libraries(${TARGET_NAME}  OpenSSL::Crypto)
  

  
  target_include_directories(
  ${TARGET_NAME} 
  PUBLIC ${OPENSSL_INCLUDE_DIR})

  message(STATUS "OPENSSL_INCLUDE_DIR: ${OPENSSL_INCLUDE_DIR}")
  message(STATUS "OPENSSL_ROOT_DIR: ${OPENSSL_ROOT_DIR}")
  message(STATUS "OPENSSL_LINK_DIR: ${OPENSSL_LINK_DIR}")

 
endif()

set(miniAudio_INCLUDE_DIRS "${LLKLiveAssist_ROOT_DIR}/Vendor/miniaudio/extras/miniaudio_split")
target_include_directories(
  ${TARGET_NAME} 
  PUBLIC ${miniAudio_INCLUDE_DIRS})

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  target_compile_options(${TARGET_NAME} PRIVATE /bigobj) #支持大文件编译
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  target_compile_options(${TARGET_NAME} PRIVATE -Wa,-mbig-obj) #支持大文件编译
  find_library(WS2_32_LIBRARY ws2_32)
  target_link_libraries(${TARGET_NAME} ws2_32) #处理wsa没有导入问题
  
endif()


