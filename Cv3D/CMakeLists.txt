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
    ../FileSystem/filesystem
)

file(GLOB BUILD_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ./Utils/*.hpp
    ./Calibration/*.hpp
    ../FileSystem/filesystem/*.hpp
)

file(GLOB BUILD_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ./Utils/*.cpp
    ./Calibration/*.cpp
    ../FileSystem/filesystem/*.cpp
)

# RapidJson
target_include_directories(${BUILD_TARGET} PRIVATE
    ../rapidjson/include
)

file(GLOB JSON_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ../rapidjson/include/*.h
)

target_sources(${BUILD_TARGET} PRIVATE
    ${BUILD_HEADERS}
    ${BUILD_SOURCES}
    ${JSON_HEADERS}
)



#######################
# Find Fundation Matrix
#######################


#######################
# Find Essential Matrix
#######################

