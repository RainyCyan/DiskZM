cmake_minimum_required(VERSION 3.5)
project(ZMD)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-mbmi2 -O3 -DNDEBUG -std=c++11")

include_directories(utils ZM)
add_executable(testZM testZM.cpp)