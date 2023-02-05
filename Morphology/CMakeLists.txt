cmake_minimum_required(VERSION 3.14.6)

# Opencv test case
include(../opencv_test_case.cmake)

make_test_case_opencv(OCV_EROSION ./erosion.cpp)
make_test_case_opencv(OCV_DILATION ./dilation.cpp)
make_test_case_opencv(OCV_CROSING ./crosing.cpp)
make_test_case_opencv(OCV_OPENING ./opening.cpp)
make_test_case_opencv(OCV_OPENING_CLOSING ./opening_closing.cpp)
