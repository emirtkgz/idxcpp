cmake_minimum_required(VERSION 4.0)

project(idxcpp)

set (CMAKE_CXX_STANDARD 17)

include_directories("../../")

add_executable(mnist_test)

target_sources(mnist_test
 PRIVATE
	main.cpp
)