cmake_minimum_required(VERSION 3.19)
project(Renderer)

set(CMAKE_CXX_STANDARD 20)

SET(CMAKE_CXX_COMPILER /opt/homebrew/Cellar/llvm/12.0.1/bin/clang-12)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(/opt/homebrew/include/eigen3)

aux_source_directory(. SRC)

set(OpenMP_CXX_FLAGS -fopenmp)
set(OpenMP_CXX_LIB_NAMES libomp)

find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -O3")
    link_directories(/opt/homebrew/Cellar/libomp/12.0.1/lib)
endif ()

add_executable(Renderer ${SRC})

target_link_libraries(Renderer /opt/homebrew/Cellar/llvm/12.0.1/lib/libc++.dylib)
target_link_libraries(Renderer ${OpenCV_LIBRARIES})
#target_link_libraries(Renderer ${omp})
