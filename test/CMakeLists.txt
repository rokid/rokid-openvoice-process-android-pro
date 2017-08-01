
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(COMMON_FLAGS "-include rokid/rokid_types.h")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS}")


add_executable(openvoice_test test.cpp)
target_link_libraries(openvoice_test android_binder)
target_link_libraries(openvoice_test android_utils android_cutils android_hardware)
target_link_libraries(openvoice_test pthread json-c)
target_link_libraries(openvoice_test bsiren fftw3f r2ad3 r2vt4 r2ssp ztvad curl opus)
target_link_libraries(openvoice_test dl speech)
target_link_libraries(openvoice_test openvoice)
