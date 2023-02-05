cmake_minimum_required(VERSION 3.14.6)

# Opencv test case
include(../opencv_test_case.cmake)

make_test_case_opencv(OCV_AVG_FILTER ./average_filter.cpp)
make_test_case_opencv(OCV_GAUSSIAN_FILTER ./gaussian_filter.cpp)
make_test_case_opencv(OCV_MEDIAN_FILTER ./median_filter.cpp)
make_test_case_opencv(OCV_WEIGHT_AVG_FILTER ./weight_average_filter.cpp)
make_test_case_opencv(OCV_BIRATERAL_FILTER ./birateral_filter.cpp)
make_test_case_opencv(OCV_SHARPNESS_FILTER ./sharpening_filter.cpp)
