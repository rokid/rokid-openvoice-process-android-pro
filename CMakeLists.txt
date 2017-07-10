cmake_minimum_required(VERSION 2.8)
project(runtime)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I.. -Iinclude -std=c++11" )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I.. -Iinclude -std=c++11" )


set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(COMMON_FLAGS "-include rokid/rokid_types.h")
include (TargetArch)
target_architecture(DETECTED_ARCH)
if (DETECTED_ARCH MATCHES "armv8")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS}")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS}")
elseif (DETECTED_ARCH MATCHES "arm.*")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS} -DBINDER_IPC_32BIT")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS} -DBINDER_IPC_32BIT")
endif()

aux_source_directory(./src DIR_SRCS)

add_executable(runtime ${DIR_SRCS})
target_link_libraries(runtime android_binder)
target_link_libraries(runtime android_utils android_cutils android_hardware)
target_link_libraries(runtime bsiren fftw3f r2ad3 r2vt4 r2ssp ztvad curl opus)
target_link_libraries(runtime pthread json-c)
target_link_libraries(runtime dl speech)
