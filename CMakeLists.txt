project(OpenCvExamples)
cmake_minimum_required(VERSION 2.8)

include(cmake_stuff/options.cmake               REQUIRED)
include(cmake_stuff/findDependencies.cmake      REQUIRED)
include(cmake_stuff/compileroptions.cmake       REQUIRED)

add_subdirectory(apps)

include(cmake_stuff/printInfo.cmake             REQUIRED)
