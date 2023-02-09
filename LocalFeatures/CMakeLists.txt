cmake_minimum_required(VERSION 3.14.6)

# Opencv test case
include(../opencv_test_case.cmake)

make_test_case_opencv(OCV_LF_EIGEN_VALUE_KEYPOINT ./ev_keypoint.cpp)
make_test_case_opencv(OCV_LF_HARRIS_KEYPOINT ./harris_keypoint.cpp)
make_test_case_opencv(OCV_LF_FAST_KEYPOINT ./fast_keypoint.cpp)
make_test_case_opencv(OCV_LF_AGAST_KEYPOINT ./agast_keypoint.cpp)
make_test_case_opencv(OCV_LF_MSER_KEYPOINT ./mser_keypoint.cpp)
make_test_case_opencv(OCV_LF_BRISK_KEYPOINT_DESCRIPTOR ./brisk_keypoint_descriptor.cpp)
make_test_case_opencv(OCV_LF_KAZE_KEYPOINT_DESCRIPTOR ./kaze_keypoint_descriptor.cpp)
make_test_case_opencv(OCV_LF_ORB_KEYPOINT_DESCRIPTOR ./orb_keypoint_descriptor.cpp)
make_test_case_opencv(OCV_LF_AKAZE_KEYPOINT_DESCRIPTOR ./akaze_keypoint_descriptor.cpp)


# make_test_case_opencv(OCV_LF_STAR_KEYPOINT ./star_keypoint.cpp) # need opencv-contrib