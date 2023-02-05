cmake_minimum_required(VERSION 3.14.6)

# Opencv test case
include(../opencv_test_case.cmake)

make_test_case_opencv(OCV_CANNY_EDGE_DETECTOR ./canny_edge_detector.cpp)
make_test_case_opencv(OCV_LAPLACIAN_EDGE_DETECTOR ./laplacian_edge_detector.cpp)
make_test_case_opencv(OCV_DIFFERENCIAL_EDGE_DETECTOR ./differential_edge_detector.cpp)
make_test_case_opencv(OCV_SOBEL_EDGE_DETECTOR ./sobel_edge_detector.cpp)

