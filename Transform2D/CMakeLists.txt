cmake_minimum_required(VERSION 3.14.6)

# Opencv test case
include(../opencv_test_case.cmake)

make_test_case_opencv(OCV_AFFINE_2D ./affine_2d.cpp)
make_test_case_opencv(OCV_PERSPECTIVE_2D ./perspective_2d.cpp)
make_test_case_opencv(OCV_INTERPOLATION_2D ./interpolation_2d.cpp)