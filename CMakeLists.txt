# Specify the minimum version for CMake

cmake_minimum_required(VERSION 3.5.1)

add_compile_options(-std=c++14)

project (detectorORB VERSION 0.1 LANGUAGES CXX)
#Bring the headers into the project
include_directories(include)
 
#Bring sources into the project:
file(GLOB SOURCES "src/*.cpp")

find_package( OpenCV REQUIRED )
add_executable(detectorORB ${SOURCES})
target_link_libraries( detectorORB ${OpenCV_LIBS} )