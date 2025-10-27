
file(GLOB TEST_SOURCES *.cpp *.h)

# Fetch GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY  https://github.com/google/googletest.git
    GIT_TAG         v1.17.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


enable_testing()
add_executable(test_main "${TEST_SOURCES}")
target_link_libraries(test_main cppcoda_lib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(test_main)
