cmake_minimum_required(VERSION 3.22)
project(memory_manage)

set(CMAKE_CXX_STANDARD 17)

add_executable(memory_manage main.cpp MYSTL/SmallObjectAllocator.cpp leetcode.cpp )

include_directories(E:\\A_Code)