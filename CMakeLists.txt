cmake_minimum_required(VERSION 3.19)
project(Renderer)

set(CMAKE_CXX_STANDARD 20)

#SET(CMAKE_CXX_COMPILER /opt/homebrew/Cellar/llvm/12.0.1/bin/clang-12)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

#SET(CMAKE_CXX_COMPILER /opt/homebrew/Cellar/gcc/11.1.0/bin/aarch64-apple-darwin20-g++-11)

#SET(LIBCXX_INC_PATH /opt/homebrew/Cellar/llvm/12.0.0/include/c++/v1)
#SET(LIBCXX_LIB_PATH /opt/homebrew/Cellar/llvm/12.0.0/lib)

#SET(LIBCXX_INC_PATH /opt/homebrew/Cellar/gcc/10.2.0_4/include/c++/10.2.1)
#SET(LIBCXX_LIB_PATH /opt/homebrew/Cellar/gcc/10.2.0_4/lib/gcc/10/libstdc++.dylib)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

#include_directories(/opt/homebrew/include/GLFW)
#include_directories(/opt/homebrew/include/GL)
#include_directories(/opt/homebrew/include)

include_directories(/opt/homebrew/include/eigen3)

## glew的库目录路径
#link_directories(/opt/homebrew/Cellar/glew/2.2.0_1/lib)
## glfw的库目录路径
#link_directories(/opt/homebrew/Cellar/glfw/3.3.4/lib)

#link_directories(/opt/homebrew/Cellar/eigen)

aux_source_directory(. SRC)

#set(OpenMP_CXX_FLAGS -fopenmp)
#set(OpenMP_CXX_LIB_NAMES libomp)
#
#find_package(OpenMP REQUIRED)
#if (OPENMP_FOUND)
#    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -O3")
#    link_directories(/opt/homebrew/Cellar/libomp/12.0.1/lib)
#endif ()

add_executable(Renderer ${SRC})

#target_link_libraries(Depth_Search /opt/homebrew/Cellar/llvm/12.0.1/lib/libc++.dylib)
target_link_libraries(Renderer ${OpenCV_LIBRARIES})
#target_link_libraries(Depth_Search ${omp})
## 指定glew库
#target_link_libraries(Depth_Search GLEW)
## 指定glfw库
#target_link_libraries(Depth_Search GLFW)

#target_link_libraries(Depth_Search "-framework OpenGL")
#target_link_libraries(Depth_Search "-framework GLUT")