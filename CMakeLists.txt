cmake_minimum_required (VERSION 3.1)
project (producer_consumer)
find_package (Threads)
add_executable(producer_consumer main.cpp)
target_link_libraries (producer_consumer ${CMAKE_THREAD_LIBS_INIT})
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
