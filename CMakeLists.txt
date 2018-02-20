cmake_minimum_required (VERSION 3.0)
project (autoprop CXX)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED TRUE)

include_directories (include)
add_subdirectory (tests)
