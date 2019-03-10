
if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
endif()

set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
set(test_libraries collections ${LIBCUTTER_LIBRARIES})
link_directories(${LIBCUTTER_LIBRARY_DIRS})

# Tests
add_library(test-example SHARED test-example.c)
target_link_libraries(test-example ${test_libraries})

