cmake_minimum_required(VERSION 3.20)
project(renderer)

set(CMAKE_CXX_STANDARD 20)

#SET(CMAKE_CXX_COMPILER /opt/homebrew/Cellar/llvm/15.0.0/bin/clang-15)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(/usr/include/eigen3)
include_directories(include)

aux_source_directory(src SRC)

#set(OpenMP_CXX_FLAGS -fopenmp)
#set(OpenMP_CXX_LIB_NAMES libomp)
#
#find_package(OpenMP REQUIRED)
#if (OPENMP_FOUND)
#    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -O3")
#    link_directories(/opt/homebrew/Cellar/llvm/13.0.0_2/lib/libomp.dylib)
#endif ()

add_executable(renderer ${SRC})

#target_link_libraries(renderer /opt/homebrew/Cellar/llvm/15.0.0/lib/c++/libc++.1.0.dylib)
target_link_libraries(renderer ${OpenCV_LIBRARIES})

#add_compile_options(-Wall -Wextra -Wpedantic -g -fsanitize=address)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")
#set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")