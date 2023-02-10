cmake_minimum_required(VERSION 3.14.6)

# Opencv test case
include(../opencv_test_case.cmake)

make_test_case_opencv(OCV_BINALIZATION ./binarize.cpp)
make_test_case_opencv(OCV_OTSU_BINARIZATION ./otsu_binarize.cpp)
make_test_case_opencv(OCV_TRIANGLE_BINARIZATION ./triangle_binarize.cpp)