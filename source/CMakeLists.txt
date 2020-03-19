# CMakeList.txt: autoupdate 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.11)

include_directories(${CMAKE_BINARY_DIR})


add_subdirectory(src)







