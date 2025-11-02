cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)

set(NAME cppcoda)

project(${NAME})

option(CPPCODA_BUILD_TESTS "Build test project" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCE *.cpp *.h)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")

#add_subdirectory(thirdparty/googletest)
add_subdirectory(cppcoda)

if (CPPCODA_BUILD_TESTS)
    add_subdirectory(test)
endif(CPPCODA_BUILD_TESTS)
