cmake_minimum_required(VERSION 3.14.6)

# Opencv test case
include(../opencv_test_case.cmake)

make_test_case_opencv(OCV_MOTION_MEANSHIFT ./tracking_meanshift.cpp)
make_test_case_opencv(OCV_MOTION_CAMSHIFT ./tracking_camshift.cpp)