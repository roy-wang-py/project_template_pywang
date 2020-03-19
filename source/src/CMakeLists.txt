cmake_minimum_required (VERSION 3.11)
project("check_camera_status")
set(CMAKE_BUILD_TYPE "Release")

set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -std=c++11 -s -O2 -Wall -fpermissive -DNDEBUG -fno-aggressive-loop-optimizations -ffunction-sections -fdata-sections -Wno-deprecated-declarations " )

if( ${CHIP_TYPE} MATCHES "x86_64" )
    set( TARGET_TYPE "x86_64")
elseif(${CHIP_TYPE} MATCHES "arm32")
    set( TARGET_TYPE "arm32")
endif()

link_directories(${PROJECT_SOURCE_DIR}/../../third_part/linux/${TARGET_TYPE}/libs)
link_directories(${PROJECT_SOURCE_DIR}/../../third_part/linux/${TARGET_TYPE}/bossdk/lib)
link_directories(${PROJECT_SOURCE_DIR}/../../third_part/linux/${TARGET_TYPE}/prometheus/lib)
link_directories(${PROJECT_SOURCE_DIR}/../../third_part/linux/${TARGET_TYPE}/gflags/lib)


# find_package(OpenMP REQUIRED)
# if(OpenMP_FOUND)
#     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#     set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
# endif()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

set(LINK_LIB  
    pthread 
    dl
    z
    png16
    uuid
    curl
    ssl
    crypto
    jsoncpp
    atomic
	libgflags.a
    ) 


set(BOOST_LIB 
    boost_system
    boost_filesystem
    )

set(PROMETHEUS_LIB
    prometheus-cpp-core
    prometheus-cpp-pull
    )

if( ${CHIP_TYPE} MATCHES "x86_64" )
	set(FFMPEG_LIB 
		avcodec
		avformat
		avutil
		swresample
		swscale
		)
	set(OPENCV_LIB 
		opencv_core 
		opencv_imgcodecs 
		opencv_imgproc 
		opencv_highgui
		opencv_video
		opencv_videoio
		opencv_features2d
		opencv_xfeatures2d
		opencv_calib3d
		) 
elseif(${CHIP_TYPE} MATCHES "arm32")
	set(FFMPEG_LIB 
		avcodec
		avformat
		avutil
		swresample
		swscale
		x264
		)
	set(OPENCV_LIB 
		opencv_core 
		opencv_imgcodecs 
		opencv_imgproc 
		opencv_highgui
		opencv_video
		opencv_videoio
		opencv_features2d
		opencv_xfeatures2d
		opencv_calib3d
		opencv_flann
		opencv_ml
		opencv_shape
		) 
endif()	
set(BOSSDK_LIB 
    libbossdk.a 
    ) 

add_definitions(-DELPP_FORCE_USE_STD_THREAD)
add_definitions(-DELPP_THREAD_SAFE)
add_definitions(-DELPP_NO_DEFAULT_LOG_FILE)
add_definitions(-DELPP_FEATURE_PERFORMANCE_TRACKING)

#include_directories(${PROJECT_SOURCE_DIR}/../../third_part/linux/${TARGET_TYPE}/boost/include)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/../../third_part/linux/${TARGET_TYPE}/boost/include)
include_directories(${PROJECT_SOURCE_DIR}/../../third_part/linux/${TARGET_TYPE}/bossdk/include)
include_directories(${PROJECT_SOURCE_DIR}/../../third_part/linux/${TARGET_TYPE}/curl/include)
include_directories(${PROJECT_SOURCE_DIR}/../../third_part/linux/${TARGET_TYPE}/ssl/include)
include_directories(${PROJECT_SOURCE_DIR}/../../third_part/linux/${TARGET_TYPE}/opencv/include)
include_directories(${PROJECT_SOURCE_DIR}/../../third_part/linux/${TARGET_TYPE}/prometheus/include)
include_directories(${PROJECT_SOURCE_DIR}/../../third_part/linux/${TARGET_TYPE}/gflags/include)
include_directories(${PROJECT_SOURCE_DIR}/../include)
include_directories(${PROJECT_SOURCE_DIR}/..)
include_directories(${PROJECT_SOURCE_DIR}/../common)
include_directories(${PROJECT_SOURCE_DIR}/../common/asio)

include_directories(${PROJECT_SOURCE_DIR}/..)
include_directories(${PROJECT_SOURCE_DIR}/json)


add_definitions(-DELPP_FORCE_USE_STD_THREAD)
add_definitions(-DELPP_THREAD_SAFE)
add_definitions(-DELPP_NO_DEFAULT_LOG_FILE)
add_definitions(-DELPP_FEATURE_PERFORMANCE_TRACKING)

include_directories(${CMAKE_BINARY_DIR})


aux_source_directory(. DEC_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/../common/ COM_SRCS)



# 将源代码添加到此项目的可执行文件。
add_executable (${PROJECT_NAME} ${DEC_SRCS} ${COM_SRCS})
target_link_libraries(${PROJECT_NAME} ${FFMPEG_LIB} ${OPENCV_LIB} ${BOSSDK_LIB} ${BOOST_LIB} ${PROMETHEUS_LIB} ${LINK_LIB})


install(TARGETS ${PROJECT_NAME} DESTINATION ${PROJECT_NAME})
#file(GLOB LINUX_LIBS "${PROJECT_SOURCE_DIR}/../../third_part/linux/${TARGET_TYPE}/libs/lib*.so") 
#install(FILES ${LINUX_LIBS}  DESTINATION check_camera_status/lib/linux/)
file(GLOB PROMETHEUS_LIBS "${PROJECT_SOURCE_DIR}/../../third_part/linux/${TARGET_TYPE}/prometheus/lib/lib*")
install(FILES ${PROMETHEUS_LIBS} DESTINATION ${PROJECT_NAME}/lib/linux/libs/)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/../../third_part/linux/${TARGET_TYPE}/libs  DESTINATION ${PROJECT_NAME}/lib/linux/)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/../../config DESTINATION ${PROJECT_NAME})
install(FILES ${PROJECT_SOURCE_DIR}/../../startup.sh DESTINATION ${PROJECT_NAME})


