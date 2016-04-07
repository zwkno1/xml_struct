
cmake_minimum_required (VERSION 3.4)

project(Test)

include_directories(.)
include_directories(../src/third_party/rapidxml)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

SET(SRC_LIST
	test.xml.cpp
	test.cpp
	)

add_executable(Test
	${SRC_LIST}
	)

