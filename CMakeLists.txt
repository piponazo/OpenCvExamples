# Note: The project is using C++17. Tested with
# Gcc 8.3 (Ubuntu 18.04)

cmake_minimum_required(VERSION 3.13)
project(OpenCvExamples LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_REQUIRED_FLAGS -std=c++17)

include(CheckCXXSymbolExists)
CHECK_CXX_SYMBOL_EXISTS(std::filesystem::path::preferred_separator filesystem cxx17fs)

include(cmake_stuff/options.cmake               REQUIRED)
include(cmake_stuff/findDependencies.cmake      REQUIRED)
include(cmake_stuff/compileroptions.cmake       REQUIRED)

add_subdirectory(apps)

include(cmake_stuff/printInfo.cmake             REQUIRED)
