cmake_minimum_required(VERSION 3.14.6)

# Opencv test case
include(../opencv_test_case.cmake)

make_test_case_opencv(OCV_CS_SPLIT_MERGE ./split_merge.cpp)
make_test_case_opencv(OCV_CS_RGB_YUV ./rgb_yuv.cpp)
make_test_case_opencv(OCV_CS_RGB_YCBCR ./rgb_ycbcr.cpp)
make_test_case_opencv(OCV_CS_RGB_HSV ./rgb_hsv.cpp)