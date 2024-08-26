cmake_minimum_required(VERSION 3.22)

project(test)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -O3")

include_directories(HdrHistogram_c/include)
add_subdirectory(HdrHistogram_c)

add_executable(pt_0 main.c)
target_link_libraries(pt_0 hdr_histogram)
target_compile_definitions(pt_0 PRIVATE NODE_PADDING=0)

add_executable(pt_64 main.c)
target_link_libraries(pt_64 hdr_histogram)
target_compile_definitions(pt_64 PRIVATE NODE_PADDING=64)

add_executable(pt_1k main.c)
target_link_libraries(pt_1k hdr_histogram)
target_compile_definitions(pt_1k PRIVATE NODE_PADDING=1024)

add_executable(pt_2k main.c)
target_link_libraries(pt_2k hdr_histogram)
target_compile_definitions(pt_2k PRIVATE NODE_PADDING=2048)

add_executable(pt_4k main.c)
target_link_libraries(pt_4k hdr_histogram)
target_compile_definitions(pt_4k PRIVATE NODE_PADDING=4096)

add_executable(pt_8k main.c)
target_link_libraries(pt_8k hdr_histogram)
target_compile_definitions(pt_8k PRIVATE NODE_PADDING=8192)
