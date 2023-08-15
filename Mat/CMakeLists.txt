cmake_minimum_required(VERSION 3.14.6)

# Opencv test case
include(../opencv_test_case.cmake)

make_test_case_opencv(OCV_MAT_GENERATE ./mat_generate.cpp)
make_test_case_opencv(OCV_MAT_ROI ./mat_roi.cpp)
