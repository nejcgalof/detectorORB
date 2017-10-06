# Specify the minimum version for CMake

cmake_minimum_required(VERSION 3.9)
project (hello)
#Bring the headers, such as Student.h into the project
include_directories(include)
 
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "src/*.cpp")
 
add_executable(testStudent ${SOURCES})