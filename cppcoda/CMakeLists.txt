
file(GLOB CPPCODA_SOURCES *.cpp *.h)

add_library(cppcoda_lib "${CPPCODA_SOURCES}")
target_include_directories(cppcoda_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")