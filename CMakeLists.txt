cmake_minimum_required(VERSION 3.22)

project(test)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -O")

include_directories(HdrHistogram_c/include)
add_subdirectory(HdrHistogram_c)

add_executable(main main.cpp)
target_link_libraries(main hdr_histogram)

