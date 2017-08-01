cmake_minimum_required(VERSION 2.8)
project(runtime)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(COMMON_FLAGS "-include rokid/rokid_types.h")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(openvoice SHARED src/IVoiceService.cpp src/callback/IVoiceCallback.cpp)
target_link_libraries(openvoice android_binder)
target_link_libraries(openvoice android_utils)

add_executable(runtime src/VoiceService.cpp src/Engine.cpp src/main.cpp)
target_link_libraries(runtime android_binder)
target_link_libraries(runtime android_utils android_cutils android_hardware)
target_link_libraries(runtime bsiren fftw3f r2ad3 r2vt4 r2ssp ztvad curl opus)
target_link_libraries(runtime pthread json-c)
target_link_libraries(runtime dl speech)
target_link_libraries(runtime openvoice)


option(BUILD_DEMO "Build the demo" ON)

if (BUILD_DEMO)
add_subdirectory(test)
endif()
