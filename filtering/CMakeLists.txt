cmake_minimum_required(VERSION 3.14.6)

# Opencv test case
include(../opencv_test_case.cmake)

make_test_case_opencv(FILTERRING_AVG_FILTER ./average_filter.cpp)