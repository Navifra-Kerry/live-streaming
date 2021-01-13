project(LIVE-STREAMING)
cmake_minimum_required (VERSION 3.5)
set(CMAKE_CXX_STANDARD 14)

include_directories(./include)
link_directories(${CMAKE_SOURCE_DIR}/build/lib)

add_definitions( -DPOCO_STATIC )

option(ENABLE_SEVENZIP OFF)
option(ENABLE_XML OFF) 
option(ENABLE_JWT ON) 
option(ENABLE_CRYPTO ON)
option(ENABLE_DATA OFF)
option(ENABLE_DATA_SQLITE OFF) 
option(ENABLE_DATA_MYSQL OFF) 
option(ENABLE_DATA_POSTGRESQL OFF) 
option(ENABLE_DATA_ODBC OFF)
option(ENABLE_MONGODB OFF)
option(ENABLE_REDIS OFF)
option(ENABLE_PDF OFF)
option(ENABLE_ZIP ON) 
option(ENABLE_APACHECONNECTOR OFF)
option(ENABLE_CPPPARSER OFF)
option(ENABLE_ENCODINGS OFF)
option(ENABLE_ENCODINGS_COMPILER OFF)
option(ENABLE_PAGECOMPILER OFF)
option(ENABLE_PAGECOMPILER_FILE2PAGE OFF) 
option(ENABLE_POCODOC OFF)
option(ENABLE_TESTS OFF)
option(ENABLE_LONG_RUNNING_TESTS OFF)
option(POCO_UNBUNDLED OFF)
option(BUILD_SHARED_LIBS OFF)

add_subdirectory(poco)
set(CMAKE_INSTALL_PREFIX "../bin")

if(WIN32)
#	set(CMAKE_PREFIX_PATH "D:/local/libtorch/")
#	set(CUDNN_INCLUDE_PATH "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v10.2/include")
endif(WIN32)

set(OpenCV_FOUND 1)
set(OpenCV_DIR "C:/local/opencv/build")
#find_package(Torch REQUIRED)
find_package(OpenCV REQUIRED)



set(SRC_FILE src/Network/router/VideoStreamingRequestHandlerFactory.cpp
             src/services/webcam/WebcamService.cpp
             src/Network/MediaTypeMapper.cpp
             src/Network/WebServerDispatcher.cpp
             src/Network/WebServerRequestHandler.cpp
             src/Network/WebServerRequestHandlerFactory.cpp
             src/LiveSubSystem.cpp
             src/main.cpp
             )




add_executable(${PROJECT_NAME} ${SRC_FILE})

if(WIN_32)
    target_link_libraries(${PROJECT_NAME} PocoFoundationmd)
    target_link_libraries(${PROJECT_NAME} PocoJSONmd)
    target_link_libraries(${PROJECT_NAME} PocoJWTmd)
    target_link_libraries(${PROJECT_NAME} PocoNetmd)
    target_link_libraries(${PROJECT_NAME} PocoUtilmd)
    target_link_libraries(${PROJECT_NAME} PocoNetSSLmd)
endif(WIN_32)

if(UNIX)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/build/lib/libPocoNet.a)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/build/lib/libPocoUtil.a)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/build/lib/libPocoJSON.a)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/build/lib/libPocoFoundation.a)
    target_link_libraries(${PROJECT_NAME} pthread)
endif(UNIX)

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} )

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)