cmake_minimum_required(VERSION 3.14.6)

# Opencv test case
include(../opencv_test_case.cmake)

# include
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/BayesFilter)

make_test_case_opencv(OCV_TRACKING_MEANSHIFT ./tracking_meanshift.cpp)
make_test_case_opencv(OCV_TRACKING_CAMSHIFT ./tracking_camshift.cpp)

make_test_case_opencv(OCV_TRACKING_PARTICLE_FILTER ./tracking_particle_filter.cpp)
make_test_case_opencv(OCV_TRACKING_LINEAR_KALMAN_FILTER ./tracking_linear_kalman_filter.cpp)