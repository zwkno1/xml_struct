
cmake_minimum_required (VERSION 3.4)

project(generator)

SET(SRC_LIST
	generator.cpp
	)

add_executable(generator 
	${SRC_LIST}
	)

