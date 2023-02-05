cmake_minimum_required(VERSION 3.14.6)


function(make_test_case_opencv TEST_TARGET TERGET_SOURCES)

    # Project
    project(${TEST_TARGET} LANGUAGES CXX VERSION 0.1.0)

    # Header
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})

    # Executable
    add_executable(${TEST_TARGET} ${TERGET_SOURCES})

    # OpenCV
    find_package(OpenCV REQUIRED)
    if(OpenCV_FOUND)
        target_include_directories(${TEST_TARGET} PRIVATE ${OpenCV_INCLUDE_DIRS})
        target_link_libraries(${TEST_TARGET} PRIVATE ${OpenCV_LIBS})
    endif()

endfunction()