cmake_minimum_required(VERSION 3.28.3)
project(cvExample2)

set(CMAKE_CXX_STANDARD 17)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(cvExample2 cv_main.cpp)
target_link_libraries(cvExample2 ${OpenCV_LIBS})