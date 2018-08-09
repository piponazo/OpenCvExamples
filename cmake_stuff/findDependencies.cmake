# Check if the conan file exist to find the dependencies
if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake REQUIRED)
    #conan_set_find_paths()
    conan_basic_setup()
endif()

find_package(OpenCV 3.4.1 REQUIRED core highgui imgproc features2d video)
#find_package(OpenCV REQUIRED)
