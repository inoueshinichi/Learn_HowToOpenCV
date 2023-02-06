cmake_minimum_required(VERSION 3.14.6)

# Opencv test case
include(../opencv_test_case.cmake)

make_test_case_opencv(OCV_ASPECT_RATIO_AREA ./aspect_ratio_area.cpp)
make_test_case_opencv(OCV_PERIMETER_ROUNDNESS ./perimeter_roundness.cpp)
make_test_case_opencv(OCV_CENTER_OF_GRAVITY ./center_of_gravity.cpp)
make_test_case_opencv(OCV_CONTOURS ./contours.cpp)