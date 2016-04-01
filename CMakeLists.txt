
cmake_minimum_required (VERSION 3.4)

project(generator)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

SET(SRC_LIST
	generator.cpp
	)

add_executable(generator 
	${SRC_LIST}
	)

