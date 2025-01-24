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







# 查找 Boost 库

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(BOOST_ROOT "${LLKLiveAssist_ROOT_DIR}/Vendor/boost/BoostBuild-GCC")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(BOOST_ROOT "${LLKLiveAssist_ROOT_DIR}/Vendor/boost/BoostBuild-MSVC")
endif()

set(Boost_INCLUDE_DIRS "${BOOST_ROOT}/include")
set(Boost_LIBRARY_DIRS "${BOOST_ROOT}/lib")




find_package(Boost 1.87 REQUIRED COMPONENTS system regex json)

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
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  target_compile_options(${TARGET_NAME} PRIVATE /bigobj) #支持大文件编译
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  target_compile_options(${TARGET_NAME} PRIVATE -Wa,-mbig-obj) #支持大文件编译
  find_library(WS2_32_LIBRARY ws2_32)
  target_link_libraries(${TARGET_NAME} ws2_32) #处理wsa没有导入问题
  
endif()
