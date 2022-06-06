cmake_minimum_required(VERSION 3.22)
project(space_filling_curves)

set(CMAKE_CXX_STANDARD 17)

find_package( OpenCV REQUIRED )
add_executable(space_filling_curves main.cpp)
include_directories( ${OpenCV_INCLUDE_DIRS} )
target_link_libraries( space_filling_curves ${OpenCV_LIBS} )