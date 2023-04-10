cmake_minimum_required(VERSION 3.14.6)

# Opencv test case
include(../opencv_test_case.cmake)

# make_test_case_opencv( "" ${CMAKE_CURRENT_SOURCE_DIR})

make_test_case_opencv(OCV_MAT_DISPLAY ./mat_display.cpp)
