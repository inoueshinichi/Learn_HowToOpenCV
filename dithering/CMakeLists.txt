cmake_minimum_required(VERSION 3.14.6)

# Opencv test case
include(../opencv_test_case.cmake)

make_test_case_opencv(OCV_RANDOM_DITHERING ./random_dithering.cpp)
make_test_case_opencv(OCV_ERROR_SCATTER_DITHERING ./error_scatter_dithering.cpp)
make_test_case_opencv(OCV_ORGANIZATION_DITHERING ./organization_dithering.cpp)