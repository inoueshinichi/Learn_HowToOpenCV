cmake_minimum_required(VERSION 3.14.6)



set(BUILD_TARGET OCV_FILESYSTEM)
set(BUILD_HEADERS "")
set(BUILD_SOURCES "")

###################
# Filesystem
###################

# Name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)

# Project
project(${PROJECT_NAME} LANGUAGES CXX VERSION 0.1.0)

# Target
set(BUILD_TARGET OCV_FILESYSTEM)
add_executable(${BUILD_TARGET} ./main_filesystem.cpp)
target_include_directories(${BUILD_TARGET} PRIVATE
    ./fileSystem
)

file(GLOB BUILD_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ./filesystem/*.hpp
)

file(GLOB BUILD_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ./filesystem/*.cpp
)

target_sources(${BUILD_TARGET} PRIVATE
    ${BUILD_HEADERS}
    ${BUILD_SOURCES}
)