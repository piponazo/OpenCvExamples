cmake_minimum_required(VERSION 3.9)
project(OpenCvExamples)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake_stuff/options.cmake               REQUIRED)
include(cmake_stuff/findDependencies.cmake      REQUIRED)
include(cmake_stuff/compileroptions.cmake       REQUIRED)

add_subdirectory(apps)

include(cmake_stuff/printInfo.cmake             REQUIRED)
