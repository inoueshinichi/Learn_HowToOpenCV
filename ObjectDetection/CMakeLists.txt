cmake_minimum_required(VERSION 3.14.6)

# Opencv test case
include(../opencv_test_case.cmake)

make_test_case_opencv(OCV_OB_FRAME_SUBTRACTION ./frame_subtraction.cpp)
make_test_case_opencv(OCV_OB_TEMPLATE_MATCHING ./template_matching.cpp)