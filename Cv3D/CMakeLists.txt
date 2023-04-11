cmake_minimum_required(VERSION 3.14.6)

# Opencv test case
include(../opencv_test_case.cmake)

set(BUILD_TARGET OCV_CALIB_ZHANG)
set(BUILD_HEADERS "")
set(BUILD_SOURCES "")

###################
# Zhang calibration
###################
set(BUILD_TARGET OCV_CALIB_ZHANG)
make_test_case_opencv(${BUILD_TARGET} ./cv3d_main_calib_zhang.cpp)

target_include_directories(${BUILD_TARGET} PRIVATE
    ./Utils
    ./Calibration
)

file(GLOB BUILD_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ./Utils/*.hpp
    ./Calibration/*.hpp
)

file(GLOB BUILD_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ./Utils/*.cpp
    ./Calibration/*.cpp
)

target_sources(${BUILD_TARGET} PRIVATE
    ${BUILD_HEADERS}
    ${BUILD_SOURCES}
)

#######################
# Find Fundation Matrix
#######################


#######################
# Find Essential Matrix
#######################

