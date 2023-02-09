cmake_minimum_required(VERSION 3.14.6)

# Opencv test case
include(../opencv_test_case.cmake)

make_test_case_opencv(OCV_ALPHA_BRENDING ./alpha_brending.cpp)
make_test_case_opencv(OCV_MASKING ./masking.cpp)
make_test_case_opencv(OCV_SUB_FG_BG ./sub_fg_bg.cpp)